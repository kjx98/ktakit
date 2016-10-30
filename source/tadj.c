/* Technical Analysis Programmer's Toolkit - Copyright (C) 1997 KTA */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include "ktakit.h"
#include "tadj.h"

short taDJfindfile(char *path, char *name, struct taDJinfo *info1);


short      taDJfindfile(char *path, char *name, struct taDJinfo *info1)
{
 short      err = 0, i1 = 0, reccnt = 0;
 char     buffer[81], shortname[9], company[16], symbol[11], equitytype, datatype;
 FILE    *fp1;
 struct taDJinfo info;
 struct taDJinfohdr infohdr;

 i1 = strlen(name);
 equitytype = toupper(name[i1 - 3]);
 datatype = toupper(name[i1 - 1]);
 strncpy(shortname, name, i1 - 4);
 shortname[i1 - 4] = '\0';

 strcpy(buffer, path);
 if (path[0] != '\0' && path[strlen(path) - 1] != ':' && path[strlen(path) - 1] != '\\')
  strcat(buffer, "\\");
 strcat(buffer, "mapinfo.dat");
 if ((fp1 = fopen(buffer, "rb")) == NULL)
  return (taErrFileOpen);
 if (fread((char *) &infohdr, sizeof(struct taDJinfohdr), 1, fp1) == 0)
 {
  fclose(fp1);
  return (taErrFileRead);
 }
 reccnt = infohdr.reccnt;
 for (i1 = 0; i1 < reccnt && !err; i1++)
 {
  if (fread((char *) &info, sizeof(struct taDJinfo), 1, fp1) == 0)
   err = taErrFileRead;
  else
  {
   sprintf(symbol, "%8.8s", info.symbol);
   sprintf(company, "%15.15s", info.company);
   taRtrim(symbol);
   taRtrim(company);
   if ((!stricmp(shortname, symbol) || !stricmp(shortname, company)) &&
    equitytype == info.equitytype && datatype == info.datatype)
   {
    memcpy(info1, &info, sizeof(struct taDJinfo));
    break;
   }
  }
 }
 fclose(fp1);
 if (i1 == reccnt)
  err = taErrFileNotFound;
 return (err);
}


long DLL_EXPORT taDJfindrec(char *path, char *name, unsigned int date, unsigned int *actualdate)
{
 short    err = 0, reclen;
 short    i1, i2;
 unsigned short dt, dt1;
 long     recno = -1, guess = 0, lastguess = 0;
 long     maxent = 0, low = 0, high = 0;
 char     buffer[81], name1[9];
 FILE    *fp1;
 struct taDJinfo info;

 if (date == 0)
  dt = 0;
 else
 {
  dt = date + taDJBASEDATE;
 }

 strcpy(name1, name);
 i2 = 1;
 while (name[(i1 = strcspn(name1, "+<>|=;\\/"))])
  name1[i1] = '0' + (i2++);
 if ((err = taDJfindfile(path, name1, &info)))
  return (err);
 if (path[0] != '\0' && path[strlen(path) - 1] != ':' && path[strlen(path) - 1] != '\\')
  sprintf(buffer, "%s\\%s", path, name1);
 else
  sprintf(buffer, "%s%s", path, name1);
 if ((fp1 = fopen(buffer, "rb")) == NULL)
  return (taErrFileOpen);

 switch (info.content)
 {
 case '1':
  reclen = sizeof(struct taDJdc);
  break;
 case '2':
  reclen = sizeof(struct taDJdvc);
  break;
 case '3':
  reclen = sizeof(struct taDJdhlc);
  break;
 case '0':
 case '4':
  reclen = sizeof(struct taDJdvhlc);
  break;
 case '5':
  reclen = sizeof(struct taDJdhlco);
  break;
 case '6':
  reclen = sizeof(struct taDJdihlco);
  break;
 case '7':
  reclen = sizeof(struct taDJdvhlco);
  break;
 case '8':
  reclen = sizeof(struct taDJdvihlco);
  break;
 case '9':
  reclen = sizeof(struct taDJdiv);
  break;
 }

 fseek(fp1, -reclen, SEEK_END);
 fread((char *) &dt1, sizeof(unsigned short), 1, fp1);
 if (dt > dt1)
 {
  fclose(fp1);
  if (actualdate)
   *actualdate = dt1 - taDJBASEDATE;
  return (taErrRecordNotFound);
 }

 maxent = ftell(fp1) / reclen;
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
  if (fseek(fp1, guess * reclen, SEEK_SET))
  {
   err = taErrFileRead;
   break;
  }
  if (!fread(&dt1, sizeof(unsigned short), 1, fp1))
  {
   err = taErrFileRead;
   break;
  }
  if (dt1 == dt || guess == 0)
  {
   recno = guess;
   break;
  }
  if (dt1 > dt)
   high = guess;
  if (dt1 < dt)
   low = guess;
  lastguess = guess;
 }
 fclose(fp1);
 if (actualdate)
   *actualdate = dt1 - taDJBASEDATE;
 if (err)
  return (err);
 return (recno);
}


int DLL_EXPORT taDJlist(char *path, char *outfile, int append)
{
 short      err = 0, i1, reccnt = 0, line = 0, page = 0;
 char     buffer[81];
 FILE    *fp1, *out;
 struct taDJinfo info;

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
 if (path[0] != '\0' && path[strlen(path) - 1] != ':' && path[strlen(path) - 1] != '\\')
  strcat(buffer, "\\");
 strcat(buffer, "mapinfo.dat");
 if ((fp1 = fopen(buffer, "rb")) == NULL)
  return (taErrFileOpen);
 if (fread((char *) &info, sizeof(struct taDJinfo), 1, fp1) == 0)
  err = taErrFileRead;
 else
  reccnt = info.reccnt;
 fprintf(out, "NUM SYMBOL   D E NAME            TAG\n");
 fprintf(out, "--- -------- - - --------------- ---\n");
 line = 2;
 for (i1 = 0; i1 < reccnt && !err; i1++)
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

  if (fread((char *) &info, sizeof(struct taDJinfo), 1, fp1) == 0)
   err = taErrFileRead;
  else
   fprintf(out, "%3i %8.8s %c %c %15.15s %3.3s\n", i1 + 1, info.symbol,
       info.datatype, info.equitytype, info.company, info.tag);
 }
 fclose(fp1);

#ifndef _WINDOWS
 if (stricmp(outfile, "STDOUT"))
#endif
  fclose(out);
 return (err);
}


int DLL_EXPORT taDJread(taBars *b1, char *path, char *name, long start, int cnt, int allocate)
{
 short    err = 0, i1, i2;
 char     buffer[81], name1[9];
 FILE    *fp1;
 struct taDJinfo info;
 struct taDJdc dc;          /* 1 */
 struct taDJdvc dvc;        /* 2 */
 struct taDJdhlc dhlc;      /* 3 */
 struct taDJdvhlc dvhlc;    /* 4 */
 struct taDJdhlco dhlco;    /* 5 */
 struct taDJdihlco dihlco;  /* 6 */
 struct taDJdvhlco dvhlco;  /* 7 */
 struct taDJdvihlco dvihlco;/* 8 */
 struct taDJdiv div;        /* 9 */

 strcpy(name1, name);
 i2 = 1;
 while (name[(i1 = strcspn(name1, "+<>|=;\\/"))])
  name1[i1] = '0' + (i2++);

 if ((err = taDJfindfile(path, name1, &info)))
  return (err);
 if (path[0] != '\0' && path[strlen(path) - 1] != ':' && path[strlen(path) - 1] != '\\')
  sprintf(buffer, "%s\\%s", path, name1);
 else
  sprintf(buffer, "%s%s", path, name1);
 if ((fp1 = fopen(buffer, "rb")) == NULL)
  return (taErrFileOpen);
 if (cnt < 0)
 {
  if (start > 0)
   cnt = min((int) info.reccnt, 16000) - start;
  else
   cnt = min((int) info.reccnt, 16000);
 }

 if (allocate)
 {
  if ((err = taAllocBars(b1, cnt)))
  {
   fclose(fp1);
   return (err);
  }
 }

 strcpy(b1->name, name);
 #ifdef OMMIT
 b1->type = taDJ;
 strcpy(b1->path, path);
 b1->reccnt = info.reccnt;
 strncpy(b1->description, info.company, 15);
 b1->description[15] = '\0';
 strncpy(b1->symbol, info.symbol, 8);
 b1->symbol[8] = '\0';
 strncpy(b1->cusip, info.cusip, 9);
 b1->cusip[9] = '\0';
 b1->begindate = info.begindt - taDJBASEDATE;
 b1->enddate = info.enddt - taDJBASEDATE;
 b1->deliverydate = 0;
 #endif
 switch (info.datatype)
 {
 case 'D':
  b1->frequency = taDAILY;
  break;
 case 'W':
  b1->frequency = taWEEKLY;
  break;
 case 'M':
  b1->frequency = taMONTHLY;
  break;
 default:
  b1->frequency = taUNKNOWN;
  break;
 }
 switch (info.equitytype)
 {
 case 'B':
 case 'T':
  b1->datatype = taBOND;
  break;
 case 'V':
  b1->datatype = taCOMMODITY;
  break;
 case 'M':
  b1->datatype = taMUTUALFUND;
  break;
 case 'O':
  b1->datatype = taOPTION;
  break;
 case 'S':
  b1->datatype = taSTOCK;
  break;
 case 'W':
 case 'X':
 case 'Y':
  b1->datatype = taINDEX;
  break;
 default:
  b1->datatype  = taUNKNOWN;
  break;
 }
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

 switch (info.content)
 {
 case '1':
  i1 = sizeof(struct taDJdc);
  break;
 case '2':
  i1 = sizeof(struct taDJdvc);
  break;
 case '3':
  i1 = sizeof(struct taDJdhlc);
  break;
 case '0':
 case '4':
  i1 = sizeof(struct taDJdvhlc);
  break;
 case '5':
  i1 = sizeof(struct taDJdhlco);
  break;
 case '6':
  i1 = sizeof(struct taDJdihlco);
  break;
 case '7':
  i1 = sizeof(struct taDJdvhlco);
  break;
 case '8':
  i1 = sizeof(struct taDJdvihlco);
  break;
 case '9':
  i1 = sizeof(struct taDJdiv);
  break;
 }

 if (fseek(fp1, start * i1, SEEK_SET))
 {
  fclose(fp1);
  return (taErrFileSeek);
 }

 i1 = 0;
 switch (info.content)
 {
 case '1':
  while (i1 < cnt && fread((char *) &dc, sizeof(struct taDJdc), 1, fp1) > 0)
  {
   taArrayItem(b1->dt, i1) = dc.dt - taDJBASEDATE;
   taArrayItem(b1->ti, i1) = 0;
   taArrayItem(b1->op, i1) = 0;
   taArrayItem(b1->hi, i1) = 0;
   taArrayItem(b1->lo, i1) = 0;
   taArrayItem(b1->cl, i1) = dc.cl;
   taArrayItem(b1->vol, i1) = 0;
   taArrayItem(b1->oi, i1) = 0;
   i1++;
  }
  break;

 case '2':
  while (i1 < cnt && fread((char *) &dvc, sizeof(struct taDJdvc), 1, fp1) > 0)
  {
   taArrayItem(b1->dt, i1) = dvc.dt - taDJBASEDATE;
   taArrayItem(b1->ti, i1) = 0;
   taArrayItem(b1->op, i1) = 0;
   taArrayItem(b1->hi, i1) = 0;
   taArrayItem(b1->lo, i1) = 0;
   taArrayItem(b1->cl, i1) = dvc.cl;
   taArrayItem(b1->vol, i1) = (float) dvc.vol;
   taArrayItem(b1->oi, i1) = 0;
   i1++;
  }
  break;

 case '3':
  while (i1 < cnt && fread((char *) &dhlc, sizeof(struct taDJdhlc), 1, fp1) > 0)
  {
   taArrayItem(b1->dt, i1) = dhlc.dt - taDJBASEDATE;
   taArrayItem(b1->ti, i1) = 0;
   taArrayItem(b1->op, i1) = 0;
   taArrayItem(b1->hi, i1) = dhlc.hi;
   taArrayItem(b1->lo, i1) = dhlc.lo;
   taArrayItem(b1->cl, i1) = dhlc.cl;
   taArrayItem(b1->vol, i1) = 0;
   taArrayItem(b1->oi, i1) = 0;
   i1++;
  }
  break;

 case '0':
 case '4':
  while (i1 < cnt && fread((char *) &dvhlc, sizeof(struct taDJdvhlc), 1, fp1) > 0)
  {
   taArrayItem(b1->dt, i1) = dvhlc.dt - taDJBASEDATE;
   taArrayItem(b1->ti, i1) = 0;
   taArrayItem(b1->op, i1) = 0;
   taArrayItem(b1->hi, i1) = dvhlc.hi;
   taArrayItem(b1->lo, i1) = dvhlc.lo;
   taArrayItem(b1->cl, i1) = dvhlc.cl;
   taArrayItem(b1->vol, i1) = (float) dvhlc.vol;
   taArrayItem(b1->oi, i1) = 0;
   i1++;
  }
  break;

 case '5':
  while (i1 < cnt && fread((char *) &dhlco, sizeof(struct taDJdhlco), 1, fp1) > 0)
  {
   taArrayItem(b1->dt, i1) = dhlco.dt - taDJBASEDATE;
   taArrayItem(b1->ti, i1) = 0;
   taArrayItem(b1->op, i1) = dhlco.op;
   taArrayItem(b1->hi, i1) = dhlco.hi;
   taArrayItem(b1->lo, i1) = dhlco.lo;
   taArrayItem(b1->cl, i1) = dhlco.cl;
   taArrayItem(b1->vol, i1) = 0;
   taArrayItem(b1->oi, i1) = 0;
   i1++;
  }
  break;

 case '6':
  while (i1 < cnt && fread((char *) &dihlco, sizeof(struct taDJdihlco), 1, fp1) > 0)
  {
   taArrayItem(b1->dt, i1) = dihlco.dt - taDJBASEDATE;
   taArrayItem(b1->ti, i1) = 0;
   taArrayItem(b1->op, i1) = dihlco.op;
   taArrayItem(b1->hi, i1) = dihlco.hi;
   taArrayItem(b1->lo, i1) = dihlco.lo;
   taArrayItem(b1->cl, i1) = dihlco.cl;
   taArrayItem(b1->vol, i1) = 0;
   taArrayItem(b1->oi, i1) = (float) dihlco.oi;
   i1++;
  }
  break;

 case '7':
  while (i1 < cnt && fread((char *) &dvhlco, sizeof(struct taDJdvhlco), 1, fp1) > 0)
  {
   taArrayItem(b1->dt, i1) = dvhlco.dt - taDJBASEDATE;
   taArrayItem(b1->ti, i1) = 0;
   taArrayItem(b1->op, i1) = dvhlco.op;
   taArrayItem(b1->hi, i1) = dvhlco.hi;
   taArrayItem(b1->lo, i1) = dvhlco.lo;
   taArrayItem(b1->cl, i1) = dvhlco.cl;
   taArrayItem(b1->vol, i1) = (float) dvhlco.vol;
   taArrayItem(b1->oi, i1) = 0;
   i1++;
  }
  break;

 case '8':
  while (i1 < cnt && fread((char *) &dvihlco, sizeof(struct taDJdvihlco), 1, fp1) > 0)
  {
   taArrayItem(b1->dt, i1) = dvihlco.dt - taDJBASEDATE;
   taArrayItem(b1->ti, i1) = 0;
   taArrayItem(b1->op, i1) = dvihlco.op;
   taArrayItem(b1->hi, i1) = dvihlco.hi;
   taArrayItem(b1->lo, i1) = dvihlco.lo;
   taArrayItem(b1->cl, i1) = dvihlco.cl;
   taArrayItem(b1->vol, i1) = (float) dvihlco.vol;
   taArrayItem(b1->oi, i1) = (float) dvihlco.oi;
   i1++;
  }
  break;

 case '9':
  while (i1 < cnt && fread((char *) &div, sizeof(struct taDJdiv), 1, fp1) > 0)
  {
   taArrayItem(b1->dt, i1) = div.dt - taDJBASEDATE;
   taArrayItem(b1->ti, i1) = 0;
   taArrayItem(b1->op, i1) = 0;
   taArrayItem(b1->hi, i1) = 0;
   taArrayItem(b1->lo, i1) = 0;
   taArrayItem(b1->cl, i1) = 0;
   taArrayItem(b1->vol, i1) = (float) div.vol;
   taArrayItem(b1->oi, i1) = (float) div.oi;
   i1++;
  }
  break;
 }
 fclose(fp1);
 taReSizeBars(b1, i1);
 return (err);
}

