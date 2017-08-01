/* Technical Analysis Programmer's Toolkit Copyright (C) 1997 KTA */

/* Calculate One Sample Program */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "ktakit.h"


int      main()
{
 int    err, samp = 3, start = 0, stop = 0, nonzero = 1;
 int    barcount = -1, allocate = 1;
 long   barstart = 0;
 KFloat closemin, closemax, closeavg, closesum, barmin, barmax;
 KFloat stdev, corcoef, disp, slope, constant;
 char   path[71] = ".";
 char   name[21] = "IBM.txt";
 char   infoname[21]= "txtInfo.txt";
 char   buffer[40];
 taBars b1;

 /* read bar data from text file */
 if ((err = taFILEread(infoname, &b1, path, name, barstart, barcount, allocate)))
 {
  taErrDesc(err, buffer);
  printf("%s Error in taFILEread\n", buffer);
  exit(err);
 }

 /* perform calculations */
 start = 0;
 stop = b1.size - 1;
 taArrayStats(&b1.cl, &closemin, &closemax, &closeavg, &closesum, start, stop, nonzero);
 taBarStats(&b1, &barmin, &barmax, start, stop);
 stdev = taStdDev(&b1.cl, samp, start, stop);
 corcoef = taCorCoef(&b1.hi, &b1.lo, samp, start, stop);
 disp = taDispersion(&b1.cl, start, stop);
 taRegressionLine(&b1.cl, &slope, &constant, start, stop);

 /* print results */
 printf("closemin  %9.3f\n", closemin);
 printf("closemax  %9.3f\n", closemax);
 printf("closeavg  %9.3f\n", closeavg);
 printf("closesum  %9.3f\n", closesum);
 printf("barmin    %9.3f\n", barmin);
 printf("barmax    %9.3f\n", barmax);
 printf("stdev     %9.3f\n", stdev);
 printf("corcoef   %9.3f\n", corcoef);
 printf("disp      %9.3f\n", disp);
 printf("slope     %9.3f\n", slope);
 printf("constant  %9.3f\n", constant);

 exit(0);
}
