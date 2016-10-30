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


struct MqlRates         rates;
struct MqlHST_header    fhdr;
int      main(int argc, char *argv[])
{
int     i, cnt;
int     barstart, barcount;
FILE    *fp;
struct tm *tmp;
time_t     dt;

 if (argc < 2 || !strcmp(argv[1], "/?"))
 {
  fputs("Technical Analysis Programmer's Toolkit, Copyright (c) 1997 KTA\n", stderr);
  fputs("Sample Dump MT4 history Program\n\n", stderr);
  fputs("PARAMETERS: path [start [count]]\n", stderr);
  fputs("   Example EURUSD5.hst [0 [100]]\n", stderr);
  exit(-1);
 }
 if (argc > 2)
  barstart = atoi(argv[2]); else barstart = 0;
 if (argc > 3)
  barcount = atoi(argv[3]); else barcount = 100;

 printf("Size short: %d, Size int: %d, Size long: %d, Size float: %d, Size double: %d\n", (int)sizeof(short), (int)sizeof(int),
        (int)sizeof(long),(int)sizeof(float),(int)sizeof(double));
 printf("Size long long: %d, Size long double: %d, Size time_t: %d\n", (int)sizeof(long long), (int)sizeof(long double),(int)sizeof(time_t));
 printf("Size byte: %d, Size word: %d, Size dword: %d, Size qword: %d, size MqlRate: %d\n", (int)sizeof(byte), (int)sizeof(word),(int)sizeof(dword),
        (int)sizeof(qword), (int)sizeof(struct MqlRates));
 printf("64Bits: %s\n", (bit64!=0)?"true":"false");
 fp = fopen(argv[1], "rb");
 if (fp == NULL)
 {
     fprintf(stderr, "Error on open file %s\n", argv[1]);
     exit(1);
 }
 fread(&fhdr, sizeof(fhdr), 1, fp);
 i = 0;
 cnt = 0;
 printf("Symbol %s Period %d Min %d Digits\n", fhdr.c_symbol, fhdr.i_period, fhdr.i_digits);
 dt = (time_t)fhdr.i_datetime;
 tmp = gmtime(&dt);
 printf("Last update time : %04d-%02d-%02d %02d:%02d:%02d\n", tmp->tm_year+1900, tmp->tm_mon+1, tmp->tm_mday, tmp->tm_hour, tmp->tm_min, tmp->tm_sec);
 while (!feof(fp))
 {
     fread(&rates, sizeof(rates), 1, fp);
     i++;
     if (i < barstart) continue; else cnt++;
     dt = (time_t)rates.time;
     tmp = gmtime(&dt);
     if (tmp == NULL) break;
     printf("%04d%02d%02d %02d:%02d:%02d ", tmp->tm_year+1900, tmp->tm_mon+1, tmp->tm_mday, tmp->tm_hour, tmp->tm_min, tmp->tm_sec);
     printf("%7.5lf %7.5lf %7.5lf %7.5lf %ld %d %ld\n", rates.open, rates.high, rates.low, rates.close, (long)rates.tick_volume, rates.spread, (long)rates.real_volume);
     if (cnt >= barcount) break;
 }
 fclose(fp);
 exit(0);
}
