/* Technical Analysis Programmer's Toolkit - Copyright (C) 1997 KTA */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include "ktakit.h"
#include "tact.h"

short      taCTfindfile(char *path, char *name, taBars *b1, short *cols);
short      taCTreadDOP(char *path, short *cols, short *inputfactor, short *dispfactor);
unsigned short taCTtimetojulian(float time);
unsigned short taCTdatetojulian(float date);


short      taCTfindfile(char *path, char *name, taBars *b1, short *cols)
{
 short    i1, i2, found = 0, err = 0;
 char     buffer[81], cwd[81];
 FILE    *fp1;
 struct taCTmasthdr masthdr;
 struct taCTdata data;
 struct taXmasterHdr xmasterhdr;
 struct taXmaster xmaster;
 short    columns[2][3][8] = {{{1,0,0,2,3,4,5,0},{1,0,2,3,4,5,6,0},{1,0,2,3,4,5,6,7}},
                              {{1,2,0,3,4,5,6,0},{1,2,3,4,5,6,7,0},{1,2,3,4,5,6,7,8}}};

 strcpy(cwd, path);
 if (cwd[0] != '\0' && cwd[strlen(cwd) - 1] != ':' && cwd[strlen(cwd) - 1] != '\\')
  strcat(cwd, "\\");

 /* MASTER */
 strcpy(buffer, cwd);
 strcat(buffer, "master");
 if ((fp1 = fopen(buffer, "rb")) == NULL)
  err = taErrFileOpen;
 else
  if (fread((char *) &masthdr, sizeof(struct taCTmasthdr), 1, fp1) == 0)
  {
   fclose(fp1);
   err = taErrFileRead;
  }
 if (!err)
 {
  for (i1 = 0; i1 < masthdr.numentries; i1++)
  {
   if (fread((char *) &data, sizeof(struct taCTdata), 1, fp1) == 0)
   {
    fclose(fp1);
    break;
   }
   if (!strncmp(data.name + 11, "<DIR>", 5))
    continue;
   strncpy(buffer, data.name, 16);
   buffer[16] = '\0';
   taRtrim(buffer);
   if (!stricmp(name, buffer))
    found = 1;
   else
   {
    i2 = taStrNthChr(data.datasvcuse, ' ', taChrCnt(name, ' ') + 1);
    strncpy(buffer, data.datasvcuse, i2);
    buffer[i2] = '\0';
    if (!stricmp(name, buffer))
     found = 1;
    else
    {
     sprintf(buffer, "F%i.DAT", data.filenbr);
     if (!stricmp(name, buffer))
      found = 1;
    }
   }
   if (found)
   {
    fclose(fp1);
    #ifdef  OMMIT
    b1->type = taCT;
    strcpy(b1->path, path);
    sprintf(b1->name, "F%i.DAT", data.filenbr);
    strcpy(b1->description, data.name);
    strcpy(b1->symbol, data.datasvcuse);
    memset(b1->cusip, 0, 12);
    taMStoIEEE(&data.firstdate, &data.firstdate);
    taMStoIEEE(&data.lastdate, &data.lastdate);
    b1->begindate = taCTdatetojulian((long)data.firstdate);
    b1->enddate = taCTdatetojulian((long)data.lastdate);
    b1->deliverydate = 0;
    b1->strikeprice = 0;
    #endif
    b1->datatype = 0;
    b1->optiontype = 0;
    switch (data.dataformat)
    {
    case 'I':
     b1->frequency = taINTRADAY;
     break;
    case 'D':
     b1->frequency = taDAILY;
     break;
    case 'W':
     b1->frequency = taWEEKLY;
     break;
    case 'M':
     b1->frequency = taMONTHLY;
     break;
    case 'Q':
     b1->frequency = taQUARTERLY;
     break;
    case 'Y':
     b1->frequency = taYEARLY;
     break;
    default:
     b1->frequency = taUNKNOWN;
     break;
    }
    sprintf(buffer, "%sF%i.DOP", cwd, data.filenbr);
    if ((err = taReadDOPfile(buffer, cols)))
    {
     if (data.dataformat == 'I')
      memcpy(cols, columns[1][data.numfields - 6], 8 * sizeof(short));
     else
      memcpy(cols, columns[0][data.numfields - 5], 8 * sizeof(short));
    }
    return (0);
   }
  }
  fclose(fp1);
 }

 /* XMASTER */
 strcpy(buffer, cwd);
 strcat(buffer, "xmaster");
 if ((fp1 = fopen(buffer, "rb")) != NULL)
 {
  if (fread((char *) &xmasterhdr, sizeof(struct taXmasterHdr), 1, fp1) == 0)
  {
   fclose(fp1);
   err = taErrFileRead;
  }
  else
  {
   for (i1 = 0; i1 < xmasterhdr.numentries; i1++)
   {
    if (fread((char *) &xmaster, sizeof(struct taXmaster), 1, fp1) == 0)
    {
     fclose(fp1);
     break;
    }
    strncpy(buffer, data.name, 16);
    buffer[16] = '\0';
    taRtrim(buffer);
    if (!stricmp(name, xmaster.symbol))
     found = 1;
    else if (!stricmp(name, xmaster.name))
     found = 1;
    else
    {
     sprintf(buffer, "F%i.MWD", xmaster.filenbr);
     if (!stricmp(name, buffer))
      found = 1;
    }
    if (found)
    {
     fclose(fp1);
     #ifdef OMMIT
     b1->type = taCT;
     strcpy(b1->path, path);
     sprintf(b1->name, "F%i.MWD", xmaster.filenbr);
     strcpy(b1->description, xmaster.name);
     strcpy(b1->symbol, xmaster.symbol);
     memset(b1->cusip, 0, 12);
     b1->begindate = taCTdatetojulian(xmaster.firstdate);
     b1->enddate = taCTdatetojulian(xmaster.lastdate);
     b1->deliverydate = 0;
     b1->strikeprice = 0;
     #endif
     b1->datatype = 0;
     b1->optiontype = 0;
     switch (xmaster.dataformat)
     {
     case 'I':
      b1->frequency = taINTRADAY;
      break;
     case 'D':
      b1->frequency = taDAILY;
      break;
     case 'W':
      b1->frequency = taWEEKLY;
      break;
     case 'M':
      b1->frequency = taMONTHLY;
      break;
     case 'Q':
      b1->frequency = taQUARTERLY;
      break;
     case 'Y':
      b1->frequency = taYEARLY;
      break;
     default:
      b1->frequency = taUNKNOWN;
      break;
     }
     i2 = 1;
     cols[0] = i2++;
     if (xmaster.ti == 1)
      cols[1] = i2++;
     else
      cols[1] = 0;
     if (xmaster.op == 1)
      cols[2] = i2++;
     else
      cols[2] = 0;
     cols[3] = i2++;
     cols[4] = i2++;
     cols[5] = i2++;
     cols[6] = i2++;
     if (xmaster.oi == 1)
      cols[7] = i2;
     else
      cols[7] = 0;
     return (0);
    }
   }
   fclose(fp1);
  }
 }
 return (taErrFileNotFound);
}


long DLL_EXPORT taCTfindrec(char *path, char *name, unsigned int date, unsigned int time,
            unsigned int *actualdate, unsigned int *actualtime)
{
 short  i1, err = 0, cols[8], numfields = 0;
 int    year, month, day, hr, min, sec;
 long   recno = -1, guess = 0, lastguess = 0;
 long   maxent = 0, low = 0, high = 0;
 float  fields[20], dt = 0, ti = 0;
 char   buffer[81], cwd[81];
 FILE   *fp1;
 taBars   b1;
 struct taCTdathdr dathdr;

 if ((err = taCTfindfile(path, name, &b1, cols)))
  return (err);
 strcpy(cwd, path);
 if (cwd[0] != '\0' && cwd[strlen(cwd) - 1] != ':' && cwd[strlen(cwd) - 1] != '\\')
  strcat(cwd, "\\");
 strcpy(buffer, cwd);
 strcat(buffer, b1.name);
 if ((fp1 = fopen(buffer, "rb")) == NULL)
  return (taErrFileOpen);
 fread((char *) &dathdr, sizeof(struct taCTdathdr), 1, fp1);

 taJulianToYMD(date, &year, &month, &day);
 dt = (float)(year - 1900) * 10000 + month * 100 + day;
 if (cols[1])
 {
  taJulianToHMS(time, &hr, &min, &sec);
  ti = (float)hr * 10000 + min * 100 + sec;
 }
 else
  ti = 0;
 for (i1 = 0; i1 < 8; i1++)
  if (cols[i1])
   numfields++;

 fields[0] = 0;
 maxent = (long) dathdr.lastrec - 2;
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
  if (fseek(fp1, (guess + 1) * numfields * sizeof(float), SEEK_SET))
  {
   err = taErrFileRead;
   break;
  }
  if (!fread((char *) &fields[1], numfields * sizeof(float), 1, fp1))
  {
   err = taErrFileRead;
   break;
  }
  taMStoIEEE(&fields[cols[0]], &fields[cols[0]]);
  taMStoIEEE(&fields[cols[1]], &fields[cols[1]]);
  if ((fields[cols[0]] == dt && fields[cols[1]] == ti) || guess == 0)
  {
   recno = guess;
   break;
  }
  if (fields[cols[0]] > dt || (fields[cols[0]] == dt && fields[cols[1]] > ti))
   high = guess;
  if (fields[cols[0]] < dt || (fields[cols[0]] == dt && fields[cols[1]] < ti))
   low = guess;
  lastguess = guess;
 }
 fclose(fp1);
 if (fields[cols[0]] < dt || (fields[cols[0]] == dt && fields[cols[1]] < ti))
  err = taErrRecordNotFound;
 if (actualdate)
  *actualdate = taCTdatetojulian((long)fields[cols[0]]);
 if (actualtime)
  *actualtime = taCTtimetojulian((long)fields[cols[1]]);
 if (err)
  return (err);
 return (recno);
}


int DLL_EXPORT taCTlist(char *path, char *outfile, int append)
{
 short    i1 = 0, err = 0, done = 0, line = 0, page = 0;
 char     buffer[81], cwd[81];
 FILE    *fp1, *out;
 struct taCTmasthdr masthdr;
 struct taCTdata data;
 struct taXmasterHdr xmasterhdr;
 struct taXmaster xmaster;

 strcpy(cwd, path);
 if (cwd[0] != '\0' && cwd[strlen(cwd) - 1] != ':' && cwd[strlen(cwd) - 1] != '\\')
  strcat(cwd, "\\");

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

 fprintf(out, "Number Freq Symbol           Name\n");
 fprintf(out, "------ ---- ------           ----\n");
 line = 2;

 strcpy(buffer, cwd);
 strcat(buffer, "master");
 if ((fp1 = fopen(buffer, "rb")) == NULL)
  err = taErrFileOpen;
 else
  if (fread((char *) &masthdr, sizeof(struct taCTmasthdr), 1, fp1) == 0)
  {
   fclose(fp1);
   err = taErrFileRead;
  }
 if (!err)
 {
  for (i1 = 0; i1 < masthdr.numentries; i1++)
  {
   if (fread((char *) &data, sizeof(struct taCTdata), 1, fp1) == 0)
   {
    fclose(fp1);
    break;
   }
   if (!strncmp(data.name + 11, "<DIR>", 5))
    continue;
   fprintf(out, "%6u %c    %-16.16s %s\n", data.filenbr, data.dataformat, data.datasvcuse, data.name);
#ifndef _WINDOWS
   line++;
   if (page && line == 25)
   {
    printf("Press any key to continue . . .");
    getchar();
    printf("\r                               \r");
    line = 1;
   }
#endif
  }
  fclose(fp1);
 }

 strcpy(buffer, cwd);
 strcat(buffer, "xmaster");
 if ((fp1 = fopen(buffer, "rb")) != NULL)
 {
  if (fread((char *) &xmasterhdr, sizeof(struct taXmasterHdr), 1, fp1) == 0)
  {
   fclose(fp1);
   err = taErrFileRead;
  }
  else
  {
   for (i1 = 0; i1 < xmasterhdr.numentries; i1++)
   {
    if (fread((char *) &xmaster, sizeof(struct taXmaster), 1, fp1) == 0)
    {
     fclose(fp1);
     break;
    }
    fprintf(out, "%6u %c    %-16.16s %s\n", xmaster.filenbr, xmaster.dataformat, xmaster.symbol, xmaster.name);
#ifndef _WINDOWS
    line++;
    if (page && line == 25)
    {
     printf("Press any key to continue . . .");
     getchar();
     printf("\r                               \r");
     line = 1;
    }
#endif
   }
   fclose(fp1);
  }
 }

#ifndef _WINDOWS
 if (stricmp(outfile, "STDOUT"))
#endif
  fclose(out);
 return (0);
}


int DLL_EXPORT taCTread(taBars *b1, char *path, char *name, long start, int cnt, int allocate)
{
 short    i1, i2, err, recno, cols[8], numfields = 0;
 float    fields[9];
 char     buffer[81], cwd[81];
 FILE    *fp1;
 struct taCTdathdr dathdr;

 if ((err = taCTfindfile(path, name, b1, cols)))
  return (err);
 strcpy(cwd, path);
 if (cwd[0] != '\0' && cwd[strlen(cwd) - 1] != ':' && cwd[strlen(cwd) - 1] != '\\')
  strcat(cwd, "\\");
 strcpy(buffer, cwd);
 strcat(buffer, b1->name);
 if ((fp1 = fopen(buffer, "rb")) == NULL)
  return (taErrFileOpen);
 fread((char *) &dathdr, sizeof(struct taCTdathdr), 1, fp1);
 if (cnt < 0)
 {
  if (start > 0)
   cnt = min(dathdr.lastrec - 1, 16000) - start;
  else
   cnt = min(dathdr.lastrec - 1, 16000);
 }
 if (allocate)
 {
  if ((err = taAllocBars(b1, cnt)))
   return (err);
 }
 if (cnt == 0)
 {
  fclose(fp1);
  return (0);
 }
 b1->reccnt = (long) dathdr.lastrec - 1;
 cnt = min(cnt, b1->datasize);
 if (start < 0)
  start = max(0, b1->reccnt - cnt);
 if (start > dathdr.lastrec - 2)
 {
  fclose(fp1);
  return (taErrRecordNotFound);
 }
 for (i2 = 0; i2 < 8; i2++)
  if (cols[i2])
   numfields++;
 recno = start + 2;
 fseek(fp1, numfields * sizeof(float) * (start + 1), SEEK_SET);
 fields[0] = 0;
 i1 = 0;
 cnt = min(cnt, b1->datasize);

 while (i1 < cnt && recno <= dathdr.lastrec && fread((char *) &fields[1], numfields * sizeof(float), 1, fp1) > 0)
 {
  for (i2 = 1; i2 <= numfields; i2++)
   taMStoIEEE(&fields[i2], &fields[i2]);
  taArrayItem(b1->dt, i1) = taCTdatetojulian((long)fields[cols[0]]);
  taArrayItem(b1->ti, i1) = taCTtimetojulian((long)fields[cols[1]]);
  taArrayItem(b1->op, i1) = fields[cols[2]];
  taArrayItem(b1->hi, i1) = fields[cols[3]];
  taArrayItem(b1->lo, i1) = fields[cols[4]];
  taArrayItem(b1->cl, i1) = fields[cols[5]];
  taArrayItem(b1->vol, i1) = fields[cols[6]];
  taArrayItem(b1->oi, i1) = fields[cols[7]];
  i1++;
  recno++;
 }
 fclose(fp1);
 taReSizeBars(b1, i1);
 return (err);
}


unsigned short taCTtimetojulian(float time)
{
 long     hr, min, sec;

 hr = (long) (time / 10000);
 min = (long) ((time - hr * 10000) / 100);
 sec = (long) ((time - hr * 10000) - (min * 100));
 return (taHMSToJulian((unsigned short) hr, (unsigned short) min, (unsigned short) sec));
}


unsigned short taCTdatetojulian(float date)
{
 long     year, month, day;

 year = (long) (date / 10000);
 month = (long) ((date - year * 10000) / 100);
 day = (long) ((date - year * 10000 - month * 100));
 return (taYMDToJulian((unsigned short) year, (unsigned short) month, (unsigned short) day));
}
