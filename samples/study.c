/* Technical Analysis Programmer's Toolkit Copyright (C) 1997 KTA */

/* Study Sample Program */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "ktakit.h"


int      main()
{
    int      err, term = 3, start = 0, stop = 0, append = 1;
    int      barcount = -1, allocate = 1;
    long     barstart = 0;
    float    envpct = .1;
    char     path[71] = ".";
    char     name[21] = "IBM.txt";
    char     infoname[21]= "txtInfo.txt";
    char     buffer[40];
    taBars   b1;
    taArray  simplema, mahi, malo;

    /* read bar data from text file */
    if ((err = taFILEread(infoname, &b1, path, name, barstart, barcount, allocate)))
    {
        taErrDesc(err, buffer);
        printf("%s Error in taFILEread\n", buffer);
        exit(err);
    }

    /* allocate arrays to store moving averages */
    if (!(err = taAllocArray(&simplema, b1.size)))
        if (!(err = taAllocArray(&mahi, b1.size)))
            err = taAllocArray(&malo, b1.size);
    if (err)
    {
        taErrDesc(err, buffer);
        printf("%s Error in taAllocArray\n", buffer);
        exit(err);
    }

    /* calculate the simple moving average which returns an array */
    taSimpleMA(&b1.cl, &simplema, term, start);

    /* calculate the moving average envelope which returns two arrays */
    taMAEnvelope(&b1.cl, &mahi, &malo, envpct, SimpleMA, term, start);

    /* print out bar and ma data */
    start = 0;
    stop = min(b1.size - 1, 20);
    taArrayPrintf("stdout", append, start, stop,
                  "NUM.     DATE    CLOSE       MA    MA-HI    MA-LO\n",
                  "%04r %YMD/i %8.3f %8.3f %8.3f %8.3f\n",
                  b1.dt, b1.cl, simplema, mahi, malo);

    /* free bar and array memory */
    taFreeArray(&simplema);
    taFreeArray(&mahi);
    taFreeArray(&malo);
    taFreeBars(&b1);
    exit(0);
}
