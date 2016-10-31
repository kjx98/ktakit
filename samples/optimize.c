/* Technical Analysis Programmer's Toolkit - Copyright (C) 1997 KTA */

/* Optimization Sample Program */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "ktakit.h"


int      main(int argc, char *argv[])
{
 int      err = 0, crossover, term1, term2;
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
 char     heading[81];
 int      numvars = 2;
 long     count, iteration;
 struct taOptVar vars[2];

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

 /* Turn off trade-by-trade report */
 taOS.OrderParms.ttreport[0] = '\0';

 /* Delete output file since it will be opened for append */
 remove("Optimize.txt");

 /* set variable ranges */
 vars[0].from = 3;
 vars[0].to = 10;
 vars[0].incr = 1;

 vars[1].from = 18;
 vars[1].to = 25;
 vars[1].incr = 1;

 /* count number of combinations */
 count = taOptVarCount(vars, numvars);

 /* loop through number of iterations */
 for (iteration = 1; iteration <= count; iteration++)
 {
  printf("Iteration %li of %li\r", iteration, count);

  /* calculate variable values for this iteration */
  taOptVarValue(vars, numvars, iteration);

  /* calculate moving averages using variable values */
  taSimpleMA(&b1.cl, &ma1, (int)vars[0].value, 0);
  taSimpleMA(&b1.cl, &ma2, (int)vars[1].value, 0);

  /* Reset order system */
  taOrderSystemReset(&taOS);

  /* Loop through the b1 and process orders */
  while (taProcessOrders(&taOS))
  {
   /* Dont't start trading until we have moving average data */
   if (taOS.Current.barnum < (int)vars[1].value)
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

  /* Construct heading for profit report */
  sprintf(heading, "%4li %3.0f %3.0f", iteration, vars[0].value, vars[1].value);
  /* Print one line profit report to a file */
  taProfitRpt1(&taOS, "Optimize.txt", 1, heading, "nNT nAS nAT lAT sAT nPR lPR sPR");
 }
 printf("Done                \n");

 /* Close Order System */
 taOrderSystemClose(&taOS);

 /* Free Bar and Array memory */
 taFreeBars(&b1);
 taFreeArray(&ma1);
 taFreeArray(&ma2);
 exit(0);
}
