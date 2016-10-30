/* Technical Analysis Programmer's Toolkit Copyright (C) 1997 KTA */

/* Moving Functions Sample Program */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "ktakit.h"


int      main()
{
 int      err, term = 10, samp = 0, start = 0;
 int      barcount = -1, allocate = 1;
 long     barstart = 0;
 char     path[71] = ".";
 char     name[21] = "IBM.TXT";
 char     infoname[21]= "TXTINFO.TXT";
 char     buffer[40];
 taBars   b1;
 taArray  stdevmv;

 /* read bar data from text file */
 if ((err = taFILEread(infoname, &b1, path, name, barstart, barcount, allocate)))
 {
  taErrDesc(err, buffer);
  printf("%s Error in taFILEread\n", buffer);
  exit(err);
 }

 /* allocate an array to store moving standard deviation */
 if ((err = taAllocArray(&stdevmv, b1.size)))
 {
  taErrDesc(err, buffer);
  printf("%s Error in taAllocArray\n", buffer);
  exit(err);
 }

 /* calculate standard deviations */
 printf("StdDev 0- 9 = %9.4f\n", taStdDev(&b1.cl, samp, 0, 9));
 printf("StdDev 1-10 = %9.4f\n", taStdDev(&b1.cl, samp, 1, 10));
 printf("StdDev 2-11 = %9.4f\n", taStdDev(&b1.cl, samp, 2, 11));

 /* calculate moving standard deviation */
 taStdDevMv(&b1.cl, &stdevmv, samp, term, start);

 printf("StdDevMv  9 = %9.4f\n", taArrayItem(stdevmv, 9));
 printf("StdDevMv 10 = %9.4f\n", taArrayItem(stdevmv, 10));
 printf("StdDevMv 11 = %9.4f\n", taArrayItem(stdevmv, 11));

 exit(0);
}
