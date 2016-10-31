/* Technical Analysis Programmer's Toolkit - Copyright (C) 1997 KTA */

/* Trading System Sample Program */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "ktakit.h"


int      main(int argc, char *argv[])
{
 int      err = 0, crossover, term1= 5, term2 = 20;
 int      barcount = -1, allocate = 1;
 long     barstart = 0;
 char     systemname[81];
 char     path[71] = ".";
 char     name[21] = "IBM.txt";
 char     infoname[21]= "txtInfo.txt";
 char     buffer[40];
 taBars   b1;
 taArray  ma1, ma2;
 struct   taOrderSystem taOS;

 /* read bar data from text file */
 if ((err = taFILEread(infoname, &b1, path, name, barstart, barcount, allocate)))
 {
  taErrDesc(err, buffer);
  printf("%s Error in taFILEread\n", buffer);
  exit(err);
 }

 /* allocate an array to store moving average */
 if ((err = taAllocArray(&ma1, b1.size)))
 {
  taErrDesc(err, buffer);
  printf("%s Error in taAllocArray\n", buffer);
  taFreeBars(&b1);
  exit(err);
 }

 /* allocate an array to store moving average */
 if ((err = taAllocArray(&ma2, b1.size)))
 {
  taErrDesc(err, buffer);
  printf("%s Error in taAllocArray\n", buffer);
  taFreeBars(&b1);
  taFreeArray(&ma1);
  exit(err);
 }

 /* calculate moving averages */
 taSimpleMA(&b1.cl, &ma1, term1, 0);
 taSimpleMA(&b1.cl, &ma2, term2, 0);

 /* Initialize the Order System */
 sprintf(systemname, "Moving Average Cross System - %s", name);
 if ((err = taOrderSystemInit(&taOS, systemname, &b1, 50)))
 {
  taErrDesc(err, buffer);
  printf("%s Error in taOrderSystemInit\n", buffer);
  taFreeBars(&b1);
  taFreeArray(&ma1);
  taFreeArray(&ma2);
  exit(err);
 }
 strcpy(taOS.OrderParms.ttreport, "TradeRpt.txt");

 /* Loop through the bars and process orders */
 while (taProcessOrders(&taOS))
 {
  printf("Progress: %3i%%\r", (int)(100 * (taOS.Current.barnum / b1.size)));

  /* Dont't start trading until we have moving average data */
  if (taOS.Current.barnum < term2)
   continue;

  /* Evaluate buy/sell rule */
  crossover = taCrossOver(&ma1, &ma2, taOS.Current.barnum);
  if (crossover == taUP && taOS.Current.position != taLONG)
   taBuyToOpen(&taOS, "long entry", 100, 0, taMARKET, taDAY);
  else if (crossover == taDOWN && taOS.Current.position != taSHORT)
   taSellToOpen(&taOS, "short entry", 100, 0, taMARKET, taDAY);

  /* Evaluate exit rule for long trades */
  if (taOS.Current.position == taLONG && taArrayItem(b1.lo, taOS.Current.barnum) < taArrayItem(ma2, taOS.Current.barnum))
   taSellToClose(&taOS, "long exit", "", taOS.Current.totalvolume, taArrayItem(b1.lo, taOS.Current.barnum), taSTOP, taDAY);

  /* Evaluate exit rule for short trades */
  if (taOS.Current.position == taSHORT && taArrayItem(b1.hi, taOS.Current.barnum) > taArrayItem(ma2, taOS.Current.barnum))
   taBuyToClose(&taOS, "short exit", "", taOS.Current.totalvolume, taArrayItem(b1.hi, taOS.Current.barnum), taSTOP, taDAY);
 }
 printf("Progress: Done\n\n");

 /* Print one line profit report to the screen */
 printf(" AvgTradeNet AvgTradeLng AvgTradeSrt      NetPRR     LongPRR    ShortPRR\n");
 taProfitRpt1(&taOS, "STDOUT", 0, "", "nAT lAT sAT nPR lPR sPR");

 /* Print profit report to a file (append to trade-by-trade report) */
 taProfitRpt(&taOS, "TradeRpt.txt", 1);

 /* Close Order System */
 taOrderSystemClose(&taOS);

 /* Free Bar and Array memory */
 taFreeBars(&b1);
 taFreeArray(&ma1);
 taFreeArray(&ma2);
 exit(0);
}
