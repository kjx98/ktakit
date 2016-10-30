/* Technical Analysis Programmer's Toolkit Copyright (C) 1997 KTA */

/* Formation Detection Sample Program */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "ktakit.h"

int      main()
{
 int      err, pos = 0, append = 0, samp = 0, start = 0, stop = 0;
 int      barcount = -1, allocate = 1;
 long     barstart = 0;
 char     path[71] = ".";
 char     name[21] = "IBM.TXT";
 char     infoname[21]= "TXTINFO.TXT";
 char     buffer[40];
 taBars   b1;
 taArray  gaps, cprs;

 /* read bar data from text file */
 if ((err = taFILEread(infoname, &b1, path, name, barstart, barcount, allocate)))
 {
  taErrDesc(err, buffer);
  printf("%s Error in taFILEread\n", buffer);
  exit(err);
 }

 /* allocate arrays to store detected formations */
 if (!(err = taAllocArray(&gaps, b1.size)))
  err = taAllocArray(&cprs, b1.size);
 if (err)
 {
  taErrDesc(err, buffer);
  printf("%s Error in taAllocArray\n", buffer);
  exit(err);
 }

 /* fill arrays with detected formations */
 start = 0;
 stop = min(15, b1.size - 1);
 taFormationFill(&b1, &gaps, "taGap", start, stop);
 taFormationFill(&b1, &cprs, "taCPR", start, stop);

 /* print out "formation filled" arrays and  */
 /* call formation detection functions on each bar */
 printf("                                            (Filled) \n");
 printf("    DATE     OPEN     HIGH      LOW    CLOSE GAP CPR GAP CPR ISL KEY INS OUT\n");
 printf("-------- -------- -------- -------- -------- --- --- --- --- --- --- --- ---\n");
 for (pos = start; pos <= stop; pos++)
 {
  taJulianToDateTxt(taArrayItem(b1.dt, pos), DTFORMAT_YY_MM_DD, "/", buffer);
  printf("%s %8.3f %8.3f %8.3f %8.3f  %2.0f  %2.0f  %2i  %2i  %2i  %2i  %2i  %2i\n",
	 buffer,
	 taArrayItem(b1.op, pos), taArrayItem(b1.hi, pos),
	 taArrayItem(b1.lo, pos), taArrayItem(b1.cl, pos),
	 taArrayItem(gaps, pos), taArrayItem(cprs, pos),
	 taGap(&b1, pos),
	 taCPR(&b1, pos),
	 taIslandReversal(&b1, pos),
	 taKeyReversal(&b1, pos),
	 taInsideBar(&b1, pos),
	 taOutsideBar(&b1, pos));
 }

 /* free array and bar data */
 free(gaps.data);
 free(cprs.data);
 taFreeBars(&b1);
 exit(0);
}
