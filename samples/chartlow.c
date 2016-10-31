/* Technical Analysis Programmer's Toolkit Copyright (C) 1997 KTA */

/* Low Level Charting Sample Program */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <conio.h>
#ifdef _MSC_VER
 #include <graph.h>
#else
 #include <graphics.h>
 #include "ms2bgi.h"
#endif
#include "ktakit.h"


int      main()
{
 int      err = 0, x1, y1, x2, y2, xstart, xstop, width = 52;
 float    ystart = 0, ystop = 0, volmax = 0;
 int      term1 = 3, term2 = 13, term3 = 30, start = 0;
 int      barcount = -1, allocate = 1;
 long     barstart = 0;
 char     path[71] = ".";
 char     name[21] = "IBM.txt";
 char     infoname[21]= "txtInfo.txt";
 char     buffer[40];
 struct videoconfig video;
 taBars   b1;
 taArray  ma1, ma2, ma3;

 /* read bar data from text file */
 if ((err = taFILEread(infoname, &b1, path, name, barstart, barcount, allocate)))
 {
  taErrDesc(err, buffer);
  printf("%s Error in taFILEread\n", buffer);
  exit(err);
 }

 /* set video mode */
 if (!_setvideomode(_VRES16COLOR))
  if (!_setvideomode(_ERESCOLOR))
   if (!_setvideomode(_HRESBW))
    err = taChartErrNoGraphics;
 if(err)
 {
  taErrDesc(err, buffer);
  printf("%s Error in setvideomode\n", buffer);
  exit(err);
 }
 _getvideoconfig(&video);

 /* allocate arrays for three moving averages */
 if (!(err = taAllocArray(&ma1, b1.size)))
  if (!(err = taAllocArray(&ma2, b1.size)))
   err = taAllocArray(&ma3, b1.size);
 if(err)
 {
  taErrDesc(err, buffer);
  printf("%s Error in taAllocArray\n", buffer);
  exit(err);
 }

 /* calculate the moving averages */
 taSimpleMA(&b1.cl, &ma1, term1, start);
 taSimpleMA(&b1.cl, &ma2, term2, start);
 taSimpleMA(&b1.cl, &ma3, term3, start);

 /* calculate screen coordinates for charts */
 x1 = 5;
 y1 = 5;
 x2 = video.numxpixels - 6;
 y2 = video.numypixels - 6;

 /* draw rectagle around charts */
 _setcolor(taBRIGHTWHITE);
 _rectangle(_GFILLINTERIOR, 0, 0, video.numxpixels - 1, video.numypixels - 1);

 /* fill rectagle where charts will be drawn */
 _setcolor(taBLACK);
 _rectangle(_GFILLINTERIOR, x1, y1, x2, y2);

 /* calculate x and y start and stop values */
 xstart = max(b1.size - width, 0);
 xstop = b1.size - 1;
 taBarStats(&b1, &ystart, &ystop, b1.size - width, b1.size - 1);
 taArrayStats(&b1.vol, NULL, &volmax, NULL, NULL, b1.size - width, b1.size - 1, 0);

 /* draw charts and grid lines */
 taChartGrid((ystart + ystop) / 2, ystart, ystop, x1, y1, x2, y2, taRED, 0x1111);
 taChart(&b1.vol, taNULLBARS, xstart, xstop, width, 0, volmax * 4, x1, y1, x2, y2, taCHART_HIST, taBRIGHTCYAN, 0, 0x1248);
 taChart(&ma1, taNULLBARS, xstart, xstop, width, ystart, ystop, x1, y1, x2, y2, taCHART_LINE, taBRIGHTMAGENTA, 0, taSOLID);
 taChart(&ma2, taNULLBARS, xstart, xstop, width, ystart, ystop, x1, y1, x2, y2, taCHART_LINE, taYELLOW, 0, 0xFFF0);
 taChart(&ma3, taNULLBARS, xstart, xstop, width, ystart, ystop, x1, y1, x2, y2, taCHART_POINT, taBRIGHTBLUE, 0, 0);
 taChart(taNULLARRAY, &b1, xstart, xstop, width, ystart, ystop, x1, y1, x2, y2, taCHART_OPHILOCL, taBRIGHTWHITE, 0, 0);
 /* taChart(taNULLARRAY, &b1, xstart, xstop, width, ystart, ystop, x1, y1, x2, y2, taCHART_CANDLE, taBRIGHTWHITE, taBRIGHTWHITE, taBLACK); */

 /* free array and bar memory */
 free(ma3.data);
 free(ma2.data);
 free(ma1.data);
 taFreeBars(&b1);

 /* wait for a keypress */
 getch();

 /* reset vidio mode */
 _setvideomode(_DEFAULTMODE);

 exit(0);
}
