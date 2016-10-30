/* Technical Analysis Programmer's Toolkit Copyright (C) 1997 KTA */

/* Advance/Decline Sample Program */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "ktakit.h"
#include "ktamt4.h"

#ifdef  __64BIT__
int bit64=1;
#else
int bit64=0;
#endif // __64BIT__


struct MqlFXTRates      rates;
struct MqlFXT_header    fhdr;
int      main(int argc, char *argv[])
{
int     i, cnt;
int     barstart, barcount;
FILE    *fp;
long    flen;
struct tm *tmp;
time_t     dt;
char    *strp;

 if (argc < 2 || !strcmp(argv[1], "/?"))
 {
  fputs("Technical Analysis Programmer's Toolkit, Copyright (c) 1997 KTA\n", stderr);
  fputs("Sample Dump MT4 FXT history Program\n\n", stderr);
  fputs("PARAMETERS: path [start [count]]\n", stderr);
  fputs("   Example EURUSD5_0.fxt [0 [100]]\n", stderr);
  exit(-1);
 }
 if (argc > 2)
  barstart = atoi(argv[2]); else barstart = 0;
 if (argc > 3)
  barcount = atoi(argv[3]); else barcount = 100;

#ifdef  OMMIT
 printf("Size short: %d, Size int: %d, Size long: %d, Size float: %d, Size double: %d\n", (int)sizeof(short), (int)sizeof(int),
        (int)sizeof(long),(int)sizeof(float),(int)sizeof(double));
 printf("Size long long: %d, Size long double: %d, Size time_t: %d\n", (int)sizeof(long long), (int)sizeof(long double),(int)sizeof(time_t));
 // long long 8/8, long double 12/16, time_t 4/8,  32bits/64bits
 // long 4/4, float 4/4 double 4/8
 printf("Size byte: %d, Size word: %d, Size dword: %d, Size qword: %d\n", (int)sizeof(byte), (int)sizeof(word),(int)sizeof(dword),(int)sizeof(qword));
#endif
 printf("Size MqlFXTHeader: %d, size MqlFXTRate: %d, 64Bits: %s\n", (int)sizeof(fhdr), (int)sizeof(rates),(bit64!=0)?"true":"false");
 fp = fopen(argv[1], "rb");
 if (fp == NULL)
 {
     fprintf(stderr, "Error on open file %s\n", argv[1]);
     exit(1);
 }
 fseek(fp, 0, SEEK_END);
 flen = ftell(fp);
 fseek(fp, 0, SEEK_SET);
 fread(&fhdr, sizeof(fhdr), 1, fp);
 i = 0;
 cnt = 0;
 printf("FXT Version: %d, Symbol %s Period %d Min %d Digits, %d Model\n", fhdr.file_version, fhdr.c_symbol, fhdr.i_period, fhdr.i_digits, fhdr.i_model);
 printf("i_bars: %d, tick cnt: %d\n", fhdr.i_bars, (int)((flen-sizeof(fhdr))/sizeof(rates)));
 printf("i_from_bar: %d, i_to_bar: %d\n", fhdr.i_from_bar, fhdr.i_to_bar);
 printf("Model Quality: %5.2f, Spread: %d, Digits: %d\n", fhdr.d_modelquality, fhdr.i_spread, fhdr.i_digits);
 switch (fhdr.i_comm_type)
 {
    case 0: strp = "by Money"; break;
    case 1: strp = "by Pips"; break;
    case 2: strp = "by Percent"; break;
    default: strp = "none"; break;
 }
 printf("Comm Type %s per %s base %f\n", strp, (fhdr.i_comm_lots!=0)?"Deal":"Lot", fhdr.d_comm_base);
 printf("i_start_period: %d, %d, %d, ...\n", fhdr.i_start_period[0], fhdr.i_start_period[1], fhdr.i_start_period[2]);
 dt = (time_t)fhdr.t_fromdate;
 tmp = gmtime(&dt);
 printf("Tick Begin date : %04d-%02d-%02d %02d:%02d:%02d,  ", tmp->tm_year+1900, tmp->tm_mon+1, tmp->tm_mday, tmp->tm_hour, tmp->tm_min, tmp->tm_sec);
 dt = (time_t)fhdr.t_todate;
 tmp = gmtime(&dt);
 printf("Tick End date : %04d-%02d-%02d %02d:%02d:%02d\n", tmp->tm_year+1900, tmp->tm_mon+1, tmp->tm_mday, tmp->tm_hour, tmp->tm_min, tmp->tm_sec);
 dt = (time_t)fhdr.i_from;
 tmp = gmtime(&dt);
 printf("Tick i_from time : %04d-%02d-%02d %02d:%02d:%02d,  ", tmp->tm_year+1900, tmp->tm_mon+1, tmp->tm_mday, tmp->tm_hour, tmp->tm_min, tmp->tm_sec);
 dt = (time_t)fhdr.i_to;
 tmp = gmtime(&dt);
 printf("Tick i_to time : %04d-%02d-%02d %02d:%02d:%02d\n", tmp->tm_year+1900, tmp->tm_mon+1, tmp->tm_mday, tmp->tm_hour, tmp->tm_min, tmp->tm_sec);
 while (!feof(fp))
 {
     fread(&rates, sizeof(rates), 1, fp);
     i++;
     if (i < barstart) continue; else cnt++;
     dt = (time_t)rates.time;
     tmp = gmtime(&dt);
     if (tmp == NULL) break;
     printf("%04d%02d%02d %02d:%02d:%02d ", tmp->tm_year+1900, tmp->tm_mon+1, tmp->tm_mday, tmp->tm_hour, tmp->tm_min, tmp->tm_sec);
     printf("%7.5lf %7.5lf %7.5lf %7.5lf %ld %d, %d", rates.open, rates.high, rates.low, rates.close, (long)rates.tick_volume, rates.spread, rates.fill0);
     dt = (time_t)rates.last_time;
     tmp = gmtime(&dt);
     if (tmp == NULL) printf("\n"); else printf(",%02d-%02d  %02d:%02d:%02d\n", tmp->tm_mon+1, tmp->tm_mday, tmp->tm_hour, tmp->tm_min, tmp->tm_sec);
     if (cnt >= barcount) break;
 }
 fclose(fp);
 exit(0);
}
