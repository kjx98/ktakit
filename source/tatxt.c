/* Technical Analysis Programmer's Toolkit - Copyright (C) 1996 KTA */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include "ktakit.h"

long DLL_EXPORT taTXTfindrec(char *path, char *name, struct taTXTinfo *info,
              unsigned int date, unsigned int time,
              unsigned int *actualdate, unsigned int *actualtime)
{
 int      i1 = 0, maxcol = 0;
 unsigned int dt1 = 0, ti1 = 0;
 long     recno = 0, l1 = 0, l2 = 0;
 char     buffer[256], infile[81], *string;
 FILE    *fp1;

 maxcol = max(info->dt, info->ti);

 strcpy(infile, path);
#ifdef	unix
 if (infile[0] != '\0' && infile[strlen(infile) - 1] != '/')
  strcat(infile, "/");
#else
 if (infile[0] != '\0' && infile[strlen(infile) - 1] != ':' && infile[strlen(infile) - 1] != '\\')
  strcat(infile, "\\");
#endif
 strcat(infile, name);

 if ((fp1 = fopen(infile, "rt")) == NULL)
  return (taErrFileOpen);

 recno = taErrRecordNotFound;
 for (l1 = 0; fgets(buffer, 255, fp1) != NULL; l1++)
 {
  if (buffer[0] == '\n' || l1 < info->skip)
   continue;
  for (i1 = 1; i1 <= maxcol; i1++)
  {
   if (i1 == 1)
    string = strtok(buffer, info->delimit);
   else
    string = strtok(NULL, info->delimit);
   if (info->dt == i1)
    dt1 = taDateTxtToJulian(string, info->dtformat);
   if (info->ti == i1)
    ti1 = taTimeTxtToJulian(string, info->tiformat);
  }
  if (dt1 > date || (dt1 == date && ti1 >= time))
  {
   recno = l2;
   if (actualdate)
    *actualdate = dt1;
   if (actualtime)
    *actualtime = ti1;
   break;
  }
  l2++;
 }
 fclose(fp1);
 return (recno);
}


int DLL_EXPORT taTXTread(taBars *b1, char *path, char *name, struct taTXTinfo *info,
           long start, int cnt, int allocate)
{
 int    i1 = 0, i2 = 0, err = 0, maxcol = 0;
 long     reccnt = 0, fpos = 0, l1 = 0, l2 = 0;
 char     buffer[256], infile[81], *string;
 FILE    *fp1;

 maxcol = max(max(info->dt, info->ti), max(info->op, info->hi));
 maxcol = max(maxcol, max(info->lo, info->cl));
 maxcol = max(maxcol, max(info->vol, info->oi));

 strcpy(infile, path);
#ifdef	unix
 if (infile[0] != '\0' && infile[strlen(infile) - 1] != '/')
  strcat(infile, "/");
#else
 if (infile[0] != '\0' && infile[strlen(infile) - 1] != ':' && infile[strlen(infile) - 1] != '\\')
  strcat(infile, "\\");
#endif
 strcat(infile, name);

 buffer[0] = '\0';
 if ((fp1 = fopen(infile, "rt")) == NULL)
  return (taErrFileOpen);

 for (i1 = 0; i1 < info->skip; i1++)
  fgets(buffer, 255, fp1);
 do
 {
  if (l1 == start)
   fpos = ftell(fp1);
  l1++;
 }
  while (fgets(buffer, 255, fp1));
 reccnt = l1 - 1;

 if (cnt < 0)
 {
  if (start > 0)
   cnt = (int)(min(reccnt, 16000) - start);
  else
   cnt = (int) min(reccnt, 16000);
 }
 if (allocate)
 {
  if ((err = taAllocBars(b1, cnt)))
  {
   fclose(fp1);
   return (err);
  }
 }
 b1->reccnt = reccnt;
 strcpy(b1->name, name);
 #ifdef OMMIT
 b1->type = taTXT;
 strcpy(b1->path, path);
 strcpy(b1->description, name);
 memset(b1->symbol, 0, 16);
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
 {
  start = max(0, b1->reccnt - cnt);
  fpos = 0;
  fseek(fp1, 0, SEEK_SET);
  l1 = 0;
  l2 = 0;
  do
  {
   l1++;
   if (buffer[0] == '\n' || l1 < info->skip)
    continue;
   if (l2 == start)
   {
    fpos = ftell(fp1) - strlen(buffer) - 1;
    break;
   }
   l2++;
  }
   while (fgets(buffer, 255, fp1));
 }
 fseek(fp1, fpos, SEEK_SET);

 i1 = 0;
 while (i1 < cnt && fgets(buffer, 255, fp1) != NULL)
 {
  if (buffer[0] == '\n')
   continue;
  taArrayItem(b1->dt, i1) = 0;
  taArrayItem(b1->ti, i1) = 0;
  taArrayItem(b1->op, i1) = 0;
  taArrayItem(b1->hi, i1) = 0;
  taArrayItem(b1->lo, i1) = 0;
  taArrayItem(b1->cl, i1) = 0;
  taArrayItem(b1->vol, i1) = 0;
  taArrayItem(b1->oi, i1) = 0;
  for (i2 = 1; i2 <= maxcol; i2++)
  {
   if (i2 == 1)
    string = strtok(buffer, info->delimit);
   else
    string = strtok(NULL, info->delimit);
   if (info->dt == i2)
    taArrayItem(b1->dt, i1) = taDateTxtToJulian(string, info->dtformat);
   if (info->ti == i2)
    taArrayItem(b1->ti, i1) = taTimeTxtToJulian(string, info->tiformat);
   if (info->op == i2)
    taArrayItem(b1->op, i1) = atof(string);
   if (info->hi == i2)
    taArrayItem(b1->hi, i1) = atof(string);
   if (info->lo == i2)
    taArrayItem(b1->lo, i1) = atof(string);
   if (info->cl == i2)
    taArrayItem(b1->cl, i1) = atof(string);
   if (info->vol == i2)
    taArrayItem(b1->vol, i1) = atof(string);
   if (info->oi == i2)
    taArrayItem(b1->oi, i1) = atof(string);
  }
  i1++;
 }
 fclose(fp1);
 taReSizeBars(b1, i1);
 return (0);
}


int DLL_EXPORT taTXTreadinfo(char *path, char *infile, struct taTXTinfo *info)
{
 int      i1;
 char     fullname[81];
 char     buffer[81], buffer2[81], buffer3[81];
 char    *dtformat[13] = {"N/A", "YY_MM_DD", "CCYY_MM_DD", "MM_DD_YY", "MM_DD_CCYY", "DD_MM_YY", "DD_MM_CCYY",
              "YYMMDD", "CCYYMMDD", "MMDDYY", "MMDDCCYY", "DDMMYY", "DDMMCCYY"};
 char    *tiformat[5] = {"N/A", "HH_MM_SS", "HH_MM", "HHMMSS", "HHMM"};
 FILE    *fp1;

 strcpy(fullname, path);
#ifdef	unix
 if (path[0] != '\0' && path[strlen(path) - 1] != '/')
  strcat(fullname, "/");
#else
 if (path[0] != '\0' && path[strlen(path) - 1] != ':' && path[strlen(path) - 1] != '\\')
  strcat(fullname, "\\");
#endif
 strcat(fullname, infile);
 if ((fp1 = fopen(fullname, "rt")) == NULL)
  return (taErrFileOpen);
 memset(info, 0, sizeof(struct taTXTinfo));
 info->delimit[0] = ' ';
 while (fgets(buffer, 80, fp1) != NULL)
 {
  if (buffer[0] == '\n')
   continue;
  i1 = strcspn(buffer, "=");
  strncpy(buffer2, buffer, i1);
  buffer2[i1] = '\0';
  taRtrim(buffer2);
  taLtrim(buffer2);
  strcpy(buffer3, buffer + i1 + 1);
  buffer3[strlen(buffer3) - 1] = '\0';
  taLtrim(buffer3);
  taRtrim(buffer3);
  if (!stricmp(buffer2, "delimit"))
  {
   strncpy(info->delimit, buffer3, 16);
   for (i1 = 0; info->delimit[i1]; i1++)
    if (info->delimit[i1] == '_')
     info->delimit[i1] = ' ';
  }
  if (!stricmp(buffer2, "skip"))
   info->skip = atoi(buffer3);
  if (!stricmp(buffer2, "dt"))
   info->dt = atoi(buffer3);
  if (!stricmp(buffer2, "ti"))
   info->ti = atoi(buffer3);
  if (!stricmp(buffer2, "op"))
   info->op = atoi(buffer3);
  if (!stricmp(buffer2, "hi"))
   info->hi = atoi(buffer3);
  if (!stricmp(buffer2, "lo"))
   info->lo = atoi(buffer3);
  if (!stricmp(buffer2, "cl"))
   info->cl = atoi(buffer3);
  if (!stricmp(buffer2, "vol"))
   info->vol = atoi(buffer3);
  if (!stricmp(buffer2, "oi"))
   info->oi = atoi(buffer3);
  if (!stricmp(buffer2, "dtformat"))
  {
   for (i1 = 0; i1 < 13; i1++)
    if (!stricmp(buffer3, dtformat[i1]))
     info->dtformat = i1;
  }
  if (!stricmp(buffer2, "tiformat"))
  {
   for (i1 = 0; i1 < 5; i1++)
    if (!stricmp(buffer3, tiformat[i1]))
     info->tiformat = i1;
  }
 }
 fclose(fp1);
 return (0);
}


int DLL_EXPORT taTXTwriteinfo(char *path, char *outfile, struct taTXTinfo *info)
{
 int      i1;
 char     fullname[81];
 char    *dtformat[13] = {"N/A", "YY_MM_DD", "CCYY_MM_DD", "MM_DD_YY", "MM_DD_CCYY", "DD_MM_YY", "DD_MM_CCYY",
              "YYMMDD", "CCYYMMDD", "MMDDYY", "MMDDCCYY", "DDMMYY", "DDMMCCYY"};
 char    *tiformat[5] = {"N/A", "HH_MM_SS", "HH_MM", "HHMMSS", "HHMM"};
 FILE    *fp1;

 strcpy(fullname, path);
#ifdef	unix
 if (path[0] != '\0' && path[strlen(path) - 1] != '/')
  strcat(fullname, "/");
#else
 if (path[0] != '\0' && path[strlen(path) - 1] != ':' && path[strlen(path) - 1] != '\\')
  strcat(fullname, "\\");
#endif
 strcat(fullname, outfile);
 if ((fp1 = fopen(fullname, "wt")) == NULL)
  return (taErrFileOpen);
 for (i1 = 0; info->delimit[i1]; i1++)
  if (info->delimit[i1] == ' ')
   info->delimit[i1] = '_';
 fprintf(fp1, "delimit  = %s\n", info->delimit);
 fprintf(fp1, "skip     = %i\n", info->skip);
 fprintf(fp1, "dt       = %i\n", info->dt);
 fprintf(fp1, "ti       = %i\n", info->ti);
 fprintf(fp1, "op       = %i\n", info->op);
 fprintf(fp1, "hi       = %i\n", info->hi);
 fprintf(fp1, "lo       = %i\n", info->lo);
 fprintf(fp1, "cl       = %i\n", info->cl);
 fprintf(fp1, "vol      = %i\n", info->vol);
 fprintf(fp1, "oi       = %i\n", info->oi);
 fprintf(fp1, "dtformat = %s\n", dtformat[info->dtformat]);
 fprintf(fp1, "tiformat = %s\n", tiformat[info->tiformat]);
 fclose(fp1);
 return (0);
}
