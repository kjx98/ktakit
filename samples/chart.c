/* Technical Analysis Programmer's Toolkit - Copyright (C) 1997 KTA */
/* Sample Graphics Program 2 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#ifndef	unix
#include <conio.h>
#endif
#include "graphics.h"
#include "ktakit.h"


void     tradingsystem(taBars b1, taArray ma1, taArray ma2, taArray trades);
#if	!defined(_WINDOWS) && !defined(unix)
char *taChartPath = ".";
taArray  taNOARRAY = {NULL,0,0,0};
taArray *taNULLARRAY = &taNOARRAY;
taBars   taNOBARS = {NULL,0,0,0};
taBars  *taNULLBARS = &taNOBARS;
#endif

int      main(int argc, char *argv[])
{
 int    err, width = 52, barcount = 500, allocate = 1;
 int    mterm1 = 5, mterm2 = 20, sterm1 = 7, sterm2 = 3, start = 0;
 long   barstart = -1;
 KFloat volmax = 0;
 KFloat ystart, ystop, f1;
 char   *type, *path, *name;
 char   buffer[40];
 taBars   b1;
 taArray  ma1, ma2, fastd, slowd, trades;
 struct taChartScreen screen;

#ifdef  OMMIT
 if (argc < 4 || !strcmp(argv[1], "/?"))
 {
  fputs("Technical Analysis Programmer's Toolkit, Copyright (c) 1997 KTA\n", stderr);
  fputs("Sample Charting Program\n\n", stderr);
  fputs("PARAMETERS: type path name [start [count]]\n", stderr);
  fputs("   Example (CompuTrac): chart CT          C:\\KTAKIT IBM\n", stderr);
  fputs("   Example (Text file): chart txtInfo.txt C:\\KTAKIT IBM.txt\n", stderr);
  exit(-1);
 }
 type = argv[1];
 path = argv[2];
 name = argv[3];
 if (argc > 4)
  barstart = atol(argv[4]);
 if (argc > 5)
  barcount = atol(argv[5]);
#else
 type = "txtInfo.txt";
 path = ".";
 name = "IBM.txt";
#endif

 /* read bar data from file */
 if ((err = taFILEread(type, &b1, path, name, barstart, barcount, allocate)))
 {
  taErrDesc(err, buffer);
  printf("%s Error in taFILEread\n", buffer);
  exit(err);
 }

 /* allocate arrays for moving averages and stochastics */
 if (!(err = taAllocArray(&ma1, b1.size)))
  if (!(err = taAllocArray(&ma2, b1.size)))
   if (!(err = taAllocArray(&fastd, b1.size)))
    if (!(err = taAllocArray(&slowd, b1.size)))
     err = taAllocArray(&trades, b1.size);
 if (err)
 {
  taErrDesc(err, buffer);
  printf("%s Error in taAllocArray\n", buffer);
  exit(err);
 }

 /* calculate moving averages and stochastics */
 taSimpleMA(&b1.cl, &ma1, mterm1, start);
 taSimpleMA(&b1.cl, &ma2, mterm2, start);
 taFastD(&b1, &fastd, sterm1, sterm2, start);
 taSlowD(&b1, &slowd, sterm1, sterm2, sterm2, start);

 /* run trading system to fill trades array */
 tradingsystem(b1, ma1, ma2, trades);

 /* calculate max volume */
 taArrayStats(&b1.vol, NULL, &volmax, NULL, NULL, 0, b1.size - 1, 0);

 /* define chart screen */
 err = taChartScreenInit(&screen, 16, 16);
 if (err)
 {
  taErrDesc(err, buffer);
  printf("%s Error in taChartScreenInit\n", buffer);
  exit(err);
 }

 /* set chart parameters */
 strcpy(screen.heading, "^Technical Analysis Programmer's Toolkit, Copyright (c) 1996 KTA");
 sprintf(screen.footing, "%s  Keys: [L]eft [R]right [B]egin [E]nd [+]Expand [-]Compress [Q]uit", b1.name);

 /* re-set chart parameters for illustrative purposes (already set by ChartScreenInit) */
 screen.fgdcolor = taBRIGHTWHITE;
 screen.bgdcolor = taBLACK;
 screen.numcolor = taBRIGHTGREEN;
 screen.linecolor = taBRIGHTBLUE;
 screen.gridcolor = taRED;
 screen.scalepos = 1;       /* 0=off 1=left 2=right */
 screen.datestyle = 1;      /* 0=off 1=auto 2=J 3=Jan 4=January 5=31Jan */
 screen.vgrid = 0x1111;     /* linestyle, taNULLSTYLE=off */
 screen.hgrid = 0x1111;     /* linestyle, taNULLSTYLE=off */

 screen.subchartsize[0] = 3;
 screen.subchartsize[1] = 1;

 /* point to location of fonts and drivers */
 taChartPath = ".\\";

 /* define chart lines in chart #1 */
 taChartLineDef(&screen, 1, &b1.vol, taNULLBARS, 0, volmax * 4, taCHART_HIST, taBRIGHTCYAN, 0, taSOLID);
 taChartLineDef(&screen, 1, &ma1, taNULLBARS, 1, 1, taCHART_LINE, taBRIGHTMAGENTA, 0, taSOLID);
 taChartLineDef(&screen, 1, &ma2, taNULLBARS, 1, 1, taCHART_POINT, taYELLOW, 0, 1);
 taChartLineDef(&screen, 1, &trades, &b1, 0, 0, taCHART_TRADES, taBRIGHTGREEN, taBRIGHTRED, taYELLOW);
 taChartLineDef(&screen, 1, taNULLARRAY, &b1, 0, 0, taCHART_OPHILOCL, taBRIGHTWHITE, 0, 0);
 /* taChartLineDef(&screen, 1, taNULLARRAY, b1, 0, 0, taCHART_CANDLE, taBRIGHTWHITE, taBRIGHTWHITE, taBLACK); */

 /* define chart and grid lines in chart #2 */
 taChartLineDef(&screen, 2, &fastd, taNULLBARS, 0, 100, taCHART_LINE, taBRIGHTMAGENTA, 0, 0xFFFF);
 taChartLineDef(&screen, 2, &slowd, taNULLBARS, 0, 100, taCHART_LINE, taYELLOW, 0, 0xF0F0);
 taChartGridDef(&screen, 2, 80, 0, 100, taRED, screen.hgrid);
 taChartGridDef(&screen, 2, 50, 0, 100, taRED, screen.hgrid);
 taChartGridDef(&screen, 2, 20, 0, 100, taRED, screen.hgrid);

 /* call the interactive ChartDraw function */
 err = taChartDraw(&screen);
 if (err)
 {
  taErrDesc(err, buffer);
  printf("%s Error in taChartDraw\n", buffer);
 }

 /* free memory allocated by taChartScreenInit */
 taChartScreenClose(&screen);

 /* free array and bar memory */
 taFreeArray(&slowd);
 taFreeArray(&fastd);
 taFreeArray(&ma2);
 taFreeArray(&ma1);
 taFreeBars(&b1);

 exit(err);
}


/* The following trading system is the same as the one in the TRADE.C sample program */

void     tradingsystem(taBars b1, taArray ma1, taArray ma2, taArray trades)
{
 int      crossover;
 struct   taOrderSystem taOS;

 /* Initialize the Order System */
 if (taOrderSystemInit(&taOS, "Moving Average Cross System", &b1, 50))
  return;

 /* Set array to hold trades */
 taOS.tradegraph = trades;

 /* Loop through the b1 and process orders */
 while (taProcessOrders(&taOS))
 {
  /* Dont't start trading until we have moving average data */
  if (taArrayItem(ma2, taOS.Current.barnum) == 0)
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

 /* Close Order System */
 taOrderSystemClose(&taOS);
 return;
}


