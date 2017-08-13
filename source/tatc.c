/* Technical Analysis Programmer's Toolkit - Copyright (C) 1997 KTA */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include "ktakit.h"
#include "tatc.h"

unsigned    taTCDateToJulian(float date)
{
 long   year = 0, month = 0, day = 0;
 int    syr = 0, smm = 0, sday = 0;

 year = (long) (date / 10000);
 month = (long) ((date - year * 10000) / 100);
 day = (long) ((date - year * 10000) - (month * 100));
 syr = year;
 smm = month;
 sday = day;
 return (taYMDToJulian(syr, smm, sday));
}


long DLL_EXPORT taTCfindrec(char *path, char *name, unsigned int date, unsigned int *actualdate)
{
 int    err = 0;
 int    year, month, day;
 float  dt = 0;
 long   recno = -1, guess = 0, lastguess = 0;
 long   maxent = 0, low = 0, high = 0;
 char   buffer[80];
 FILE   *fp1;
 struct taTCrec rec;

 if (date == 0)
  dt = 0;
 else
 {
  taJulianToYMD(date, &year, &month, &day);
  dt = (float)(year - 1900) * 10000 + month * 100 + day;
 }

 if (path[0] != '\0' || path[strlen(path) - 1] != ':' || path[strlen(path) - 1] != '\\')
  sprintf(buffer, "%s\\%s.NDX", path, name);
 else
  sprintf(buffer, "%s%s.NDX", path, name);
 if ((fp1 = fopen(buffer, "rb")) == NULL)
  return (taErrFileOpen);
 fseek(fp1, -sizeof(struct taTCrec), SEEK_END);
 fread((char *) &rec, sizeof(struct taTCrec), 1, fp1);
 taMStoIEEE(&rec.dt, &rec.dt);
 if (dt > rec.dt)
 {
  fclose(fp1);
  if (actualdate)
   *actualdate = taTCDateToJulian(rec.dt);
  return (taErrRecordNotFound);
 }

 maxent = ftell(fp1) / sizeof(struct taTCrec) - 2;
 high = maxent;
 while (recno < 0 && !err)
 {
  guess = (low + high + 1) / 2;
  if (guess == lastguess)
  {
   if (low == 0)
    guess = 0;
   else
   {
    recno = guess;
    break;
   }
  }
  if (fseek(fp1, (guess + 2) * sizeof(struct taTCrec), SEEK_SET))
  {
   err = taErrFileRead;
   break;
  }
  if (!fread(&rec, sizeof(struct taTCrec), 1, fp1))
  {
   err = taErrFileRead;
   break;
  }
  taMStoIEEE(&rec.dt, &rec.dt);
  if (rec.dt == dt || guess == 0)
  {
   recno = guess;
   break;
  }
  if (rec.dt > dt)
   high = guess;
  if (rec.dt < dt)
   low = guess;
  lastguess = guess;
 }
 fclose(fp1);
 if (actualdate)
  *actualdate = taTCDateToJulian(rec.dt);
 if (err)
  return (err);
 return (recno);
}



int DLL_EXPORT taTClist(char *path, char *outfile, int append)
{
 int    err = 0, i1, line = 0, page = 0;
 long   cnt;
 char   buffer[81], symbol[7];
 FILE   *fp1, *out;

#ifndef _WINDOWS
 if (!stricmp(outfile, "STDOUT"))
 {
  out = stdout;
  if (append)
   page = 1;
 }
 else
#endif
 {
  if (append)
   out = fopen(outfile, "at");
  else
   out = fopen(outfile, "wt");
  if (out == NULL)
   return (taErrFileOpen);
 }
 strcpy(buffer, path);
 if (path[0] != '\0' || path[strlen(path) - 1] != ':' || path[strlen(path) - 1] != '\\')
  strcat(buffer, "\\");
 strcat(buffer, "basename");

 if ((fp1 = fopen(buffer, "rt")) == NULL)
  return (taErrFileOpen);

 fgets(symbol, 80, fp1);
 cnt = atol(symbol);             /* First entry holds count */
 fprintf(out, "SYMBOL\n");
 fprintf(out, "------\n");
 line = 2;
 for (i1 = 0; (fgets(symbol, 80, fp1) != NULL); i1++)
 {
  line++;

#ifndef _WINDOWS
  if (page && line == 25)
  {
   printf("Press any key to continue . . .");
   getchar();
   printf("\r                               \r");
   line = 1;
  }
#endif
  fprintf(out, "%3i %s", i1 + 1, symbol);
 }
 fclose(fp1);
#ifndef _WINDOWS
 if (stricmp(outfile, "STDOUT"))
#endif
  fclose(out);
 return (err);
}


int DLL_EXPORT taTCread(taBars *b1, char *path, char *name, long start, int cnt, int allocate)
{
 int    i1 = 0, err = 0;
 long   reccnt = 0;
 char   buffer[81];
 FILE   *fp1;
 struct taTCrec rec;
 struct taTChdr hdr;

 if (path[0] != '\0' || path[strlen(path) - 1] != ':' || path[strlen(path) - 1] != '\\')
  sprintf(buffer, "%s\\%s.NDX", path, name);
 else
  sprintf(buffer, "%s%s.NDX", path, name);

 if ((fp1 = fopen(buffer, "rb")) == NULL)
  return (taErrFileOpen);
 if (fread((char *) &hdr, sizeof(struct taTChdr), 1, fp1) <= 0)
 {
  fclose(fp1);
  return (taErrFileRead);
 }

 fseek(fp1, 0, SEEK_END);
 reccnt = (ftell(fp1) / sizeof(struct taTCrec)) - 2;
 fseek(fp1, 0, SEEK_SET);

 if (cnt < 0)
 {
  if (start > 0)
   cnt = min(reccnt, 16000) - start;
  else
   cnt = min(reccnt, 16000);
 }
 if (allocate)
 {
  if ((err = taAllocBars(b1, cnt)))
   return (err);
 }
 b1->reccnt = reccnt;
 strcpy(b1->name, name);
 #ifdef OMMIT
 b1->type = taTC;
 strcpy(b1->path, path);
 strcpy(b1->symbol, name);
 memset(b1->description, 0, 32);
 strncpy(b1->description, hdr.name, 22);
 memset(b1->cusip, 0, 12);
 b1->begindate = 0;
 b1->enddate = 0;
 b1->deliverydate = 0;
 #endif
 b1->frequency = 0;
 b1->datatype = 0;
 b1->optiontype = 0;
 b1->strikeprice = 0;

 if (cnt == 0)
 {
  fclose(fp1);
  return (0);
 }
 cnt = min(cnt, b1->datasize);
 if (start < 0)
  start = max(0, b1->reccnt - cnt);
 if (fseek(fp1, (start + 2) * sizeof(struct taTCrec), SEEK_SET))
 {
  fclose(fp1);
  return (taErrFileRead);
 }

 i1 = 0;
 while (i1 < cnt && fread((char *) &rec, sizeof(struct taTCrec), 1, fp1) > 0)
 {
  taMStoIEEE(&rec.dt, &rec.dt);
  taMStoIEEE(&rec.op, &rec.op);
  taMStoIEEE(&rec.hi, &rec.hi);
  taMStoIEEE(&rec.lo, &rec.lo);
  taMStoIEEE(&rec.cl, &rec.cl);
  taMStoIEEE(&rec.vol, &rec.vol);
  taArrayItem(b1->dt, i1) = taTCDateToJulian(rec.dt);
  taArrayItem(b1->ti, i1) = 0;
  taArrayItem(b1->op, i1) = rec.op;
  taArrayItem(b1->hi, i1) = rec.hi;
  taArrayItem(b1->lo, i1) = rec.lo;
  taArrayItem(b1->cl, i1) = rec.cl;
  taArrayItem(b1->vol, i1) = rec.vol;
  i1++;
 }
 fclose(fp1);
 taReSizeBars(b1, i1);
 return (err);
}
