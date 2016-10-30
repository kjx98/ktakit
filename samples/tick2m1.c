/* Technical Analysis Programmer's Toolkit Copyright (C) 1997 KTA */

/* Advance/Decline Sample Program */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "ktakit.h"

#ifdef  __64BIT__
int bit64=1;
#else
int bit64=0;
#endif // __64BIT__

struct tm   temp_tm;
char        *iopath;
char        *psymbol;
int         i_year;
FILE        *fpout;
char        filename[256];
char        linebuf[512];
int         expo[]={1,10,100,1000,10000,100000,1000000};

struct ymd
{
    int     yy;
    int     mm1, dd1;
    int     mm2, dd2;
};
struct ymd dst_range[]={{2009,3,8, 10,30}, {2010,3,14, 11,5},{2011,3,13, 11,4},
                {2012,3,11, 11,2},{2013,3,10, 11,1}, {2014,3,9, 10,31}, {2015,3,8, 10,31} };
struct ymd  *c_dstp;


BOOL    isDST(int mm, int dd)
{
   if (mm < c_dstp->mm1 || mm > c_dstp->mm2) return (FALSE);
   if (mm == c_dstp->mm1 && dd < c_dstp->dd1) return (FALSE);
   if (mm == c_dstp->mm2 && dd > c_dstp->dd2) return (FALSE);
   return (TRUE);
}



void    write1bar(FILE *fpout, time_t bartime, int digits, double op, double hi, double lo, double cl, int vol, int spread)
{
struct tm *tmp;
    tmp=localtime(&bartime);
    fprintf(fpout, "%04d%02d%02d,%02d:%02d,%7.*f,%7.*f,%7.*f,%7.*f,%d,%d\n", tmp->tm_year+1900, tmp->tm_mon+1, tmp->tm_mday,
    tmp->tm_hour, tmp->tm_min, digits, op, digits, hi, digits, lo, digits, cl, vol, spread);
}
int      main(int argc, char *argv[])
{
int     i, cnt;
FILE    *fp;
int     digits;
struct tm *tmp;
time_t     dt;

#ifdef  _DEBUG
 iopath = "d:\\qqdownload\\forex\\tickdata";
 psymbol = "EURUSD";
 i_year = 2009;
 digits = 5;
#else
 if (argc < 4)
 {
  fputs("Technical Analysis Programmer's Toolkit, Copyright (c) 1997 KTA\n", stderr);
  fputs("Convert TickData to M1\n\n", stderr);
  fputs("PARAMETERS: path symbol year\n", stderr);
  fputs("   Example d:\forex-data EURUSD 2005\n", stderr);
  exit(-1);
 }
 iopath = argv[1];
 psymbol = argv[2];
 i_year = atoi(argv[3]);
 if (argc > 4) digits = atoi(argv[4]); else digits = 5;
 if (digits < 0 || digits > 5) digits = 0;
#endif
 if (i_year >= 2009 && i_year < 2016) c_dstp = dst_range + (i_year-2009); else
 {
     fprintf(stderr, "Year %d out of range\n", i_year);
     exit(1);
 }
#ifdef  unix
 sprintf(filename, "%s/%s-%04d.csv", iopath, psymbol, i_year);
#else
 sprintf(filename, "%s\\%s-%04d.csv", iopath, psymbol, i_year);
#endif
 fpout = fopen(filename, "w+t");
 if (fpout == NULL)
 {
     fprintf(stderr, "Open output file %s error\n", filename);
 }
 cnt = 0;
 time(&dt);
 tmp = gmtime(&dt);
 memcpy(&temp_tm, tmp, sizeof(*tmp));
 for(i=1;i<=12;i++)
 {
     double     op, hi, lo, cl, bid, ask;
     time_t     bartime, tmp_time;
     int        spread, vol;
#ifdef  unix
    sprintf(filename, "%s/%s-%04d-%02d.csv", iopath, psymbol, i_year, i);
#else
    sprintf(filename, "%s\\%s-%04d-%02d.csv", iopath, psymbol, i_year, i);
#endif

    fp = fopen(filename, "rt");
    if (fp == NULL) continue;
    bartime = 0;
    while (fgets(linebuf, sizeof(linebuf), fp) != NULL)
    {
        char    *pstr;
        int     y,mon,d,h,min,s;
        pstr = strtok(linebuf, ",");
        if (pstr == NULL) continue;
        // first for symbol
        pstr = strtok(NULL, ",");
        if (pstr == NULL) continue;
        sscanf(pstr, "%04d%02d%02d %02d:%02d:%02d", &y, &mon, &d, &h, &min, &s);
        temp_tm.tm_year = y - 1900;
        temp_tm.tm_mon = mon -1;
        temp_tm.tm_mday = d;
        temp_tm.tm_hour = h;
        temp_tm.tm_min = min;
        temp_tm.tm_sec = s;
        tmp_time = mktime(&temp_tm);
        // adjust time from GMT to europe EST/EDST
        if (isDST(mon, d)) tmp_time += 3600*3; else tmp_time += 3600*2;
        pstr = strtok(NULL, ",");
        if (pstr == NULL) continue;
        bid = atof(pstr);
        pstr = strtok(NULL, ",");
        if (pstr == NULL) continue;
        ask = atof(pstr);
        if (bartime != 0 && bartime + 900 < tmp_time)
        {
            printf("Lost quote > 15 min, %02d%02d %02d:%02d:%02d\n", mon, d, h, min, s);
        }
        if (bartime != 0 && bartime + 60 <= tmp_time)
        {
            write1bar(fpout, bartime, digits, op, hi, lo, cl, vol, spread);
            bartime = 0;
        }
        if (bartime == 0)
        {
            op = hi = lo = cl = bid;
            vol = 1;
            bartime = tmp_time;
            bartime -= (tmp_time % 60);
        } else
        {
            vol++;
            cl = bid;
            if (bid > hi) hi = bid;
            if (bid < lo) lo = bid;
        }
        spread = (int)((ask-bid)*expo[digits]);
    }
    write1bar(fpout, bartime, digits, op, hi, lo, cl, vol, spread);
    fclose(fp);
    cnt++;
 }
 fclose(fpout);
 printf("Read %d tickdata files\n", cnt);
 exit(0);
}
