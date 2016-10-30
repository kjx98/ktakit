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


int      main()
{
 int      err = 0, append = 1, term1 = 3, term2 = 13, start = 0, stop = 0;
 int      pos = 0, skip = 4, startcol = 2, stopcol = 6;
 char     infile[81] = "advdec.txt", delimit[2] = " ";
 char     buffer[40];
 taArray  adv, dec, unc, advvol, decvol;
 taArray  armsidx, bolton, mcosc, mcsum;

 /* read advance/decline data from text file into multi-dimentional array */
 if ((err = taTXT2Array(infile, &adv, delimit, skip, startcol, stopcol)))
 {
  taErrDesc(err, buffer);
  printf("%s Error in taTXT2Array\n", buffer);
  exit(err);
 }

 printf("Size short: %d, Size int: %d, Size long: %d, Size float: %d, Size double: %d\n", (int)sizeof(short), (int)sizeof(int),
        (int)sizeof(long),(int)sizeof(float),(int)sizeof(double));
 printf("Size long long: %d, Size long double: %d, Size time_t: %d\n", (int)sizeof(long long), (int)sizeof(long double),(int)sizeof(time_t));
 printf("Size byte: %d, Size word: %d, Size dword: %d, Size qword: %d\n", (int)sizeof(byte), (int)sizeof(word),(int)sizeof(dword),(int)sizeof(qword));
 printf("64Bits: %s\n", (bit64!=0)?"true":"false");
 /* print advance/decline data (from multi-dimentional array) */
 stop = min(adv.size - 1, 9);
 printf("NUM.      ADV      DEC      UNC   ADVVOL   DECVOL\n");
 for (pos = 0; pos <= stop; pos++)
  printf("%04i %8.0lf %8.0lf %8.0lf %8.0lf %8.0lf\n", pos, taArrayItemM(adv, 0, pos),
	 taArrayItemM(adv, 1, pos), taArrayItemM(adv, 2, pos),
	 taArrayItemM(adv, 3, pos), taArrayItemM(adv, 4, pos));

 /* create other a/d arrays by pointing to multi-dimentional array */
 dec = adv;
 dec.element = 1;
 unc = adv;
 unc.element = 2;
 advvol = adv;
 advvol.element = 3;
 decvol = adv;
 decvol.element = 4;

 /* allocate arrays to store indicators */
 if (!(err = taAllocArray(&armsidx, adv.size)))
  if (!(err = taAllocArray(&bolton, adv.size)))
   if (!(err = taAllocArray(&mcosc, adv.size)))
    err = taAllocArray(&mcsum, adv.size);
 if (err)
 {
  taErrDesc(err, buffer);
  printf("%s Error in taAllocArray\n", buffer);
  exit(err);
 }

 /* calculate indicators */
 taArmsIdx(&adv, &dec, &advvol, &decvol, &armsidx, start, stop);
 taBoltonTremblay(&adv, &dec, &unc, &bolton, start);
 taMcClellanOscSum(&adv, &dec, &mcosc, &mcsum, term1, term2, start);

 /* print indicator data */
 taArrayPrintf("stdout", append, start, stop,
	       "\nNUM.  ARMSIDX   BOLTON    MCOSC    MCSUM\n",
	       "%04r %8.3lf %8.3lf %8.3lf %8.3lf\n",
	       armsidx, bolton, mcosc, mcsum);
 exit(0);
}
