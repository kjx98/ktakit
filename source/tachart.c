/* Technical Analysis Programmer's Toolkit - Copyright (C) 1997 KTA */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#ifndef	unix
#include <conio.h>
#endif
#include "graphics.h"
#include "ktakit.h"

char *taChartPath = ".";

int    taChartCoord(KFloat level, KFloat ystart, KFloat ystop, int y1, int y2);
KFloat    taChartRoundMinMax(int y1, int y2, KFloat ymin, KFloat ymax, KFloat *ystart, KFloat *ystop, int fontheight);


int DLL_EXPORT   taChart(taArray *pa1, taBars *pb1, int xstart, int xstop, int xwidth,
           KFloat ystart, KFloat ystop, int x1, int y1, int x2, int y2,
           int type, long color1, long color2, long style)
{
 int    err = 0, i1 = 0, pos, x3, y3, y4;
 KFloat    yscale, xscale, halfxscale;
 unsigned char fill[8];
 taArray    a1;
 taBars     b1;
 //struct videoconfig config;
 //struct xycoord points[4];
#ifdef _MSC_VER
 unsigned long linestyle1, linestyle2;
#endif

 a1 = *pa1;
 b1 = *pb1;

 if (type == taCHART_LINE && taIsNoArray(a1) && !taIsNoBars(b1))
  a1 = b1.cl;
 if (type == taCHART_HILOCL || type == taCHART_OPHILOCL || type == taCHART_CANDLE || type == taCHART_TRADES)
 {
  if (taIsNoBars(b1))
   return (taErrInvalidParm);
  xstop = min(xstop, b1.size - 1);
 }
 if (type == taCHART_HIST || type == taCHART_LINE || type == taCHART_POINT)
 {
  if (taIsNoArray(a1))
   return (taErrInvalidParm);
  xstop = min(xstop, a1.size - 1);
 }
 if (ystop == ystart || xstop == xstart)
  return (taErrInvalidParm);
 xstop = min(xstop, xstart + xwidth - 1);
 xscale = (KFloat)(x2 - x1 + 1) / xwidth;
 yscale = (KFloat)(y2 - y1) / (ystop - ystart);
 x1 += ((x2 - x1 + 1) - (int)(xwidth * xscale)) / 2;
 halfxscale = (xscale - 2) / 2;
 setcolor((int)color1);

 //setviewport(x1, y1, x2, y2, 0);
 y2 -= y1;

 switch (type)
 {
 case taCHART_HILOCL:
 case taCHART_OPHILOCL:
  for (pos = xstart; pos <= xstop && pos < b1.size; pos++)
  {
   //x3 = (pos - xstart) * xscale + halfxscale + 1;
   x3 = (pos - xstart) * xscale + halfxscale + x1;
   moveto(x3, y2 - (int) ((taArrayItem(b1.hi, pos) - ystart) * yscale));
   lineto(x3, y2 - (int) ((taArrayItem(b1.lo, pos) - ystart) * yscale));
   moveto(x3 + 1, y2 - (int) ((taArrayItem(b1.cl, pos) - ystart) * yscale));
   lineto(x3 + halfxscale, y2 - (int) ((taArrayItem(b1.cl, pos) - ystart) * yscale));
   if (type == taCHART_OPHILOCL && taArrayItem(b1.op, pos))
   {
    moveto(x3 - 1, y2 - (int) ((taArrayItem(b1.op, pos) - ystart) * yscale));
    lineto(x3 - halfxscale, y2 - (int) ((taArrayItem(b1.op, pos) - ystart) * yscale));
   }
  }
  break;

 case taCHART_CANDLE:
  if (fmod(xscale, 2))
   i1 = 3;
  else
   i1 = 2;
  for (pos = xstart; pos <= xstop && pos < b1.size; pos++)
  {
   //x3 = (pos - xstart) * xscale + 1;
   x3 = (pos - xstart) * xscale + x1;
   moveto(x3 + halfxscale, y2 - (int) ((taArrayItem(b1.hi, pos) - ystart) * yscale));
   lineto(x3 + halfxscale, y2 - (int) ((taArrayItem(b1.lo, pos) - ystart) * yscale));
   if (!taArrayItem(b1.op, pos) || taArrayItem(b1.op, pos) == taArrayItem(b1.cl, pos))
   {
    moveto(x3, y2 - (int) ((taArrayItem(b1.cl, pos) - ystart) * yscale));
    lineto(x3 + xscale - i1, y2 - (int) ((taArrayItem(b1.cl, pos) - ystart) * yscale));
    continue;
   }
   if (taArrayItem(b1.op, pos) > taArrayItem(b1.cl, pos)) /* down */
    setcolor((int)style);
   else                                                   /*  up  */
    setcolor((int)color2);
   rectangle(x3 + 1, y2 - (int) ((taArrayItem(b1.cl, pos) - ystart) * yscale),
	  x3 - 1 + xscale - i1, y2 - (int) ((taArrayItem(b1.op, pos) - ystart) * yscale));
   setcolor((int)color1);
   rectangle(x3, y2 - (int) ((taArrayItem(b1.cl, pos) - ystart) * yscale),
	  x3 + xscale - i1, y2 - (int) ((taArrayItem(b1.op, pos) - ystart) * yscale));
  }
  break;

 case taCHART_HIST:
  if (style != taSOLID)
  {
   fill[0] = (unsigned char) (style >> 12);
   fill[1] = (unsigned char) ((style & 0x0F00) >> 8);
   fill[2] = (unsigned char) ((style & 0x00F0) >> 4);
   fill[3] = (unsigned char) (style & 0x000F);
   for (i1 = 0; i1 < 4; i1++)
    fill[i1] += fill[i1] << 4;
   for (i1 = 4; i1 < 8; i1++)
    fill[i1] = fill[i1 - 4];
   //setfillmask(fill);
  }
  y3 = (int) (y2 + ystart * yscale);
  for (pos = xstart; pos <= xstop && pos < a1.size; pos++)
  {
   //x3 = (pos - xstart) * xscale + 1;
   x3 = (pos - xstart) * xscale + x1;
   rectangle(x3, y3, x3 + xscale - 2, y2 - (int) ((taArrayItem(a1, pos) - ystart) * yscale));
   if (style != taSOLID)
    rectangle(x3, y3, x3 + xscale - 2, y2 - (int) ((taArrayItem(a1, pos) - ystart) * yscale));
  }
  //setfillmask(NULL);
  break;

 case taCHART_LINE:
  if (style != taSOLID)
  {
#ifdef _MSC_VER
   linestyle1 = style + (unsigned long) style * 65536;
#endif
   setlinestyle((int)style,0,0);
  }
  for (pos = xstart + 1; pos <= xstop && pos < a1.size && !taArrayItem(a1, pos - 1); pos++);
  if (pos > xstop || pos == a1.size)
  {
   setlinestyle(taSOLID,0,0);
   break;
  }
  moveto((pos - xstart - 1) * xscale + 1, y2 - (int) ((taArrayItem(a1, pos - 1) - ystart) * yscale));
  for (; pos <= xstop && pos < a1.size; pos++)
  {
   //x3 = (pos - xstart) * xscale + halfxscale;
   x3 = (pos - xstart) * xscale + halfxscale + x1;
#ifdef _MSC_VER
   if (style != taSOLID)
   {
    linestyle2 = linestyle1 >> 16 - (x3 & 15);
    setlinestyle((unsigned) linestyle2);
   }
#endif
   lineto(x3, y2 - (int) ((taArrayItem(a1, pos) - ystart) * yscale));
  }
  setlinestyle(taSOLID,0,0);
  break;

 case taCHART_POINT:
  for (pos = xstart; pos <= xstop && pos < a1.size && !taArrayItem(a1, pos); pos++);
  for (; pos <= xstop && pos < a1.size; pos++)
  {
   //x3 = (pos - xstart) * xscale + halfxscale + 1;
   x3 = (pos - xstart) * xscale + halfxscale + x1;
   y3 = y2 - (int) ((taArrayItem(a1, pos) - ystart) * yscale);
   //setpixel(x3, y3);
  }
  break;

 case taCHART_FILL:
#ifdef _MSC_VER
  for (pos = xstart + 1; pos <= xstop && pos < a1.size && !taArrayItem(a1, pos - 1); pos++);
  if (pos > xstop || pos == a1.size)
   break;
  points[0].xcoord = (pos - xstart - 1) * xscale + 1;
  points[0].ycoord = y2 - (int) ((taArrayItem(a1, pos - 1) - ystart) * yscale);
  points[1].xcoord = points[0].xcoord;
  points[1].ycoord = y2;
  points[2].ycoord = y2;
  for (; pos <= xstop && pos < a1.size; pos++)
  {
   points[3].xcoord = (pos - xstart) * xscale + halfxscale;
   points[3].ycoord = y2 - (int) ((taArrayItem(a1, pos) - ystart) * yscale);
   points[2].xcoord = points[3].xcoord;
   _polygon(_GFILLINTERIOR, points, 4);
   points[0] = points[3];
   points[1] = points[2];
  }
  setfillmask(NULL);
#else
  err = taErrInvalidParm;
#endif
  break;

 case taCHART_TRADES:
  for (pos = xstart; pos <= xstop && pos < a1.size; pos++)
  {
   i1 = (int) taArrayItem(a1, pos);
   if (i1 == 0)
    continue;
   //x3 = (pos - xstart) * xscale + halfxscale + 1;
   x3 = (pos - xstart) * xscale + halfxscale + x1;
   y3 = y2 - (int) ((taArrayItem(b1.hi, pos) - ystart) * yscale + 2);
   y4 = y2 - (int) ((taArrayItem(b1.lo, pos) - ystart) * yscale - 2);
   if (i1 & taBO)
   {
    setcolor((int)color1);
    moveto(x3, y4 + xscale);
    lineto(x3, y4);
    lineto(x3 + halfxscale, y4 + halfxscale);
    moveto(x3, y4);
    lineto(x3 - halfxscale, y4 + halfxscale);
   }
   if (i1 & taBC)
   {
    setcolor((int)style);
    moveto(x3, y4 + xscale * 2);
    lineto(x3, y4 + xscale);
    lineto(x3 + halfxscale, y4 + xscale + halfxscale);
    moveto(x3, y4 + xscale);
    lineto(x3 - halfxscale, y4 + xscale + halfxscale);
    moveto(x3 - halfxscale, y4 + xscale * 2);
    lineto(x3 + halfxscale, y4 + xscale * 2);
   }
   if (i1 & taSO)
   {
    setcolor((int)color2);
    moveto(x3, y3 - xscale);
    lineto(x3, y3);
    lineto(x3 + halfxscale, y3 - halfxscale);
    moveto(x3, y3);
    lineto(x3 - halfxscale, y3 - halfxscale);
   }
   if (i1 & taSC)
   {
    setcolor((int)style);
    moveto(x3, y3 - xscale * 2);
    lineto(x3, y3 - xscale);
    lineto(x3 + halfxscale, y3 - xscale - halfxscale);
    moveto(x3, y3 - xscale);
    lineto(x3 - halfxscale, y3 - xscale - halfxscale);
    moveto(x3 - halfxscale, y3 - xscale * 2);
    lineto(x3 + halfxscale, y3 - xscale * 2);
   }
  }
  break;

 default:
  err = taErrInvalidParm;
  break;
 }
 //_getvideoconfig(&config);
 //setviewport(0, 0, getmaxx(), getmaxy(), 0);
 return (err);
}


int DLL_EXPORT   taChartGrid(KFloat level, KFloat ystart, KFloat ystop,
	     int x1, int y1, int x2, int y2, long color, long style)
{
 KFloat    y3, yscale;

 yscale = (y2 - y1) / (ystop - ystart);
 y3 = y2 - (level - ystart) * yscale;
 if (y3 > y2 || y3 < y1)
  return (0);
 setcolor((int)color);
 setlinestyle((int)style,0,0);
 moveto(x1, (int) y3);
 lineto(x2, (int) y3);
 setlinestyle(taSOLID,0,0);
 return (0);
}


int  DLL_EXPORT  taChartScreenInit(struct taChartScreen *screen, int linemax, int gridmax)
{
 {
    int gd, gm;
    detectgraph(&gd, &gm);
#ifdef	unix
    gm = SDL_1024x768;
#else
    // for use openbgi
    gd = VGA;
    gm = GM_1024x768;
#endif
    initgraph(&gd, &gm, "SHOW_INVISIBLE_PAGE");
 }
 memset(screen, 0, sizeof(struct taChartScreen));
 screen->chartline = (struct taChartLine *)malloc(linemax * sizeof(struct taChartLine));
 if (screen->chartline == NULL)
  return (taErrMalloc);
 screen->chartgrid = (struct taChartGrids *)malloc(gridmax * sizeof(struct taChartGrids));
 if (screen->chartgrid == NULL)
 {
  free(screen->chartline);
  return (taErrMalloc);
 }
 screen->linemax = linemax;
 screen->gridmax = gridmax;
 screen->defined = taCHART_DEFINED;
 screen->fgdcolor = taBLACK;
 screen->bgdcolor = taBRIGHTWHITE;
 screen->numcolor = taBLACK;
 screen->linecolor = taBLACK;
 screen->gridcolor = taGRAY;
 screen->scalepos = 1;
 screen->datestyle = 1;
 screen->vgrid = taDOT;
 screen->hgrid = taDOT;
 return(0);
}


void DLL_EXPORT    taChartScreenClose(struct taChartScreen *screen)
{
 free(screen->chartline);
 free(screen->chartgrid);
 closegraph();
 return;
}


int DLL_EXPORT taChartLineDef(struct taChartScreen *screen, int chartnum,
	    taArray *a1, taBars *b1, KFloat ystart, KFloat ystop,
	    int type, long color1, long color2, long style)
{
 if (screen->defined != taCHART_DEFINED)
  return(taChartErrUndefined);
 if (screen->linecnt >= screen->linemax)
  return(taChartErrMaxlines);

 screen->chartline[screen->linecnt].chartnum = chartnum - 1;
 screen->chartline[screen->linecnt].a1 = *a1;
 screen->chartline[screen->linecnt].b1 = *b1;
 screen->chartline[screen->linecnt].ystart = ystart;
 screen->chartline[screen->linecnt].ystop = ystop;
 screen->chartline[screen->linecnt].type = type;
 screen->chartline[screen->linecnt].color1 = color1;
 screen->chartline[screen->linecnt].color2 = color2;
 screen->chartline[screen->linecnt].style = style;
 screen->linecnt++;
 return(0);
}


int DLL_EXPORT taChartGridDef(struct taChartScreen *screen,
	    int chartnum, KFloat value, KFloat ystart, KFloat ystop,
	    long color, long style)
{
 if (screen->defined != taCHART_DEFINED)
  return(taChartErrUndefined);
 if (screen->gridcnt >= screen->gridmax)
  return(taChartErrMaxgrids);

 screen->chartgrid[screen->gridcnt].chartnum = chartnum - 1;
 screen->chartgrid[screen->gridcnt].value = value;
 screen->chartgrid[screen->gridcnt].ystart = ystart;
 screen->chartgrid[screen->gridcnt].ystop = ystop;
 screen->chartgrid[screen->gridcnt].color = color;
 screen->chartgrid[screen->gridcnt].style = style;
 screen->gridcnt++;
 return(0);
}


int DLL_EXPORT   taChartSetup(struct taChartScreen *screen, struct taChartInfo *info)
{
 int    i1, i2, i3;

 if (screen->defined != taCHART_DEFINED || !screen->linecnt)
  return (taChartErrUndefined);

 info->top = screen->y1 + 1;
 info->bottom = screen->y2 - 1;
 info->left = screen->x1 + 1;
 info->right = screen->x2 - 1;

 if (screen->scalepos == 1)
 {
  info->scaleleft = info->left;
  info->left += info->fontwidth * 6 + 1;
  info->scaleright = info->left;
 }
 else if (screen->scalepos == 2)
 {
  info->scaleright = info->right + 2;
  info->right -= info->fontwidth * 6 + 1;
  info->scaleleft = info->right + 2;
 }

 setcolor((int)screen->bgdcolor);
 rectangle(screen->x1, screen->y1, screen->x2, screen->y2);
 setcolor((int)screen->linecolor);

 if (screen->heading[0])
 {
  if (screen->heading[0] == '^')
  {
   i1 = ((screen->x2 - screen->x1 + 1) - strlen(screen->heading + 1) * info->fontwidth) / 2;
   i2 = 1;
  }
  else
  {
   i1 = 1;
   i2 = 0;
  }
  setcolor((int)screen->fgdcolor);
  moveto(screen->x1 + i1, info->top);
  outtext(screen->heading + i2);
  info->top += info->fontheight + 1;
 }
 if (screen->footing[0])
 {
  if (screen->footing[0] == '^')
  {
   i1 = ((screen->x2 - screen->x1 + 1) - strlen(screen->footing + 1) * info->fontwidth) / 2;
   i2 = 1;
  }
  else
  {
   i1 = 1;
   i2 = 0;
  }
  info->bottom -= (taChrCnt(screen->footing, '\n') + 1) * info->fontheight + 1;
  setcolor((int)screen->fgdcolor);
  moveto(screen->x1 + i1, info->bottom + 2);
  outtext(screen->footing + i2);
  setcolor((int)screen->linecolor);
  moveto(screen->x1, info->bottom + 1);
  lineto(screen->x2, info->bottom + 1);
 }
 if (screen->datestyle)
  info->bottom -= info->fontheight + 1;

 setcolor((int)screen->linecolor);
 rectangle(screen->x1, screen->y1, screen->x2, screen->y2);
 moveto(screen->x1, info->top - 1);
 lineto(screen->x2, info->top - 1);
 moveto(screen->x1, info->bottom + 1);
 lineto(screen->x2, info->bottom + 1);
 if (screen->scalepos)
 {
  moveto(info->scaleleft - 1, info->top);
  lineto(info->scaleleft - 1, info->bottom);
  moveto(info->scaleright - 1, info->top);
  lineto(info->scaleright - 1, info->bottom);
 }

 for (i1 = 0; i1 < screen->linecnt; i1++)
 {
  if (!taIsNoBars(screen->chartline[i1].b1) && screen->subchartsize[screen->chartline[i1].chartnum] < 2)
   screen->subchartsize[screen->chartline[i1].chartnum] = 3;
  else if (screen->subchartsize[screen->chartline[i1].chartnum] == 0)
   screen->subchartsize[screen->chartline[i1].chartnum] = 1;
 }
 for (i1 = 0, i2 = 0; i1 < taCHART_SUBCNTMAX; i1++)
  i2 += screen->subchartsize[i1];
 i2 = (info->bottom - info->top + 1) / i2;
 i3 = 0;
 info->subtop[0] = info->top;
 for (i1 = 1; i1 < taCHART_SUBCNTMAX; i1++)
 {
  info->subtop[i1] = info->subtop[i1 - 1] + screen->subchartsize[i1 - 1] * i2;
  if (screen->subchartsize[i1])
  {
   moveto(screen->x1, info->subtop[i1]);
   lineto(screen->x2, info->subtop[i1]);
   info->subtop[i1]++;
   i3 = i1;
  }
 }
 info->subtop[i3 + 1] = info->bottom + 2;

 for (i1 = 0; i1 < screen->linecnt; i1++)
 {
  if (screen->chartline[i1].ystart == 0 && screen->chartline[i1].ystop == 0)
  {
   if (!taIsNoBars(screen->chartline[i1].b1))
    taBarStats(&screen->chartline[i1].b1, &screen->chartline[i1].ystart, &screen->chartline[i1].ystop, 0, screen->chartline[i1].b1.size - 1);
   else
    taArrayStats(&screen->chartline[i1].a1, &screen->chartline[i1].ystart, &screen->chartline[i1].ystop, NULL, NULL, 0, screen->chartline[i1].a1.size - 1, 0);
  }
  if (screen->chartline[i1].ystart != screen->chartline[i1].ystop)
   taChartRoundMinMax(info->subtop[screen->chartline[i1].chartnum],
     info->subtop[screen->chartline[i1].chartnum + 1] - 2,
     screen->chartline[i1].ystart, screen->chartline[i1].ystop,
     &screen->chartline[i1].ystart, &screen->chartline[i1].ystop,
     info->fontheight);
 }
 for (i1 = 0; i1 < taCHART_SUBCNTMAX; i1++)
  info->subystart[i1] = info->subystop[i1] = 0;
 for (i1 = 0; i1 < screen->linecnt; i1++)
 {
  if (!taIsNoBars(screen->chartline[i1].b1) && screen->chartline[i1].ystart != screen->chartline[i1].ystop)
  {
   info->subystart[screen->chartline[i1].chartnum] = screen->chartline[i1].ystart;
   info->subystop[screen->chartline[i1].chartnum] = screen->chartline[i1].ystop;
  }
  else if (info->subystart[screen->chartline[i1].chartnum] == 0 && info->subystop[screen->chartline[i1].chartnum] == 0 &&
       screen->chartline[i1].ystart != screen->chartline[i1].ystop)
  {
   info->subystart[screen->chartline[i1].chartnum] = screen->chartline[i1].ystart;
   info->subystop[screen->chartline[i1].chartnum] = screen->chartline[i1].ystop;
  }
 }
 for (i1 = 0; i1 < screen->linecnt; i1++)
 {
  if (screen->chartline[i1].ystart == 1 && screen->chartline[i1].ystop == 1)
  {
   screen->chartline[i1].ystart = info->subystart[screen->chartline[i1].chartnum];
   screen->chartline[i1].ystop = info->subystop[screen->chartline[i1].chartnum];
  }
 }
 for (i1 = 0, info->xmax = 0; i1 < screen->linecnt; i1++)
 {
  if (!taIsNoBars(screen->chartline[i1].b1))
  {
   info->xmax = max(info->xmax, screen->chartline[i1].b1.size);
   if (taIsNoBars(info->masterbars))
    info->masterbars = screen->chartline[i1].b1;
  }
  else
   info->xmax = max(info->xmax, screen->chartline[i1].a1.size);
 }
 if (!taIsNoBars(info->masterbars))
  info->xmax = min(info->xmax, info->masterbars.size);
 if (screen->xwidth == 0)
  screen->xwidth = (info->right - info->left + 1) / 4;
 screen->xstop = info->xmax - 1;
 screen->xstart = max(0, screen->xstop - screen->xwidth + 1);
 setviewport(info->left, info->top, info->right, info->bottom, 0);
 //setviewport(screen->x1, info->top, screen->x2, info->bottom, 0);
return (0);
}


int  DLL_EXPORT  taChartPaint(struct taChartScreen *screen, struct taChartInfo *info)
{
 int    i1, i2, i3, i4;
 int    yr, mo, da;
 KFloat    f1, f2;
 char     buffer[81];
 char    *months[3][12] = {
  {"J", "F", "M", "A", "M", "J", "J", "A", "S", "O", "N", "D"},
  {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"},
  {"January", "February", "March", "April", "May", "June",
  "July", "August", "September", "October", "November", "December"},
 };

 setcolor((int)screen->bgdcolor);
 rectangle(info->left, info->top, info->right, info->bottom);
 i1 = info->right - info->left + 1;
 info->xscale = (int) floor(i1 / screen->xwidth);
 info->dateleft = info->left + (i1 - screen->xwidth * (int) info->xscale) / 2;
 if (screen->datestyle && !taIsNoBars(info->masterbars) && (taArrayItem(info->masterbars.dt, screen->xstop) - taArrayItem(info->masterbars.dt, screen->xstart)) >= 1)
 {
  if (screen->datestyle == 1)
  {
   f1 = (taArrayItem(info->masterbars.dt, screen->xstop) - taArrayItem(info->masterbars.dt, screen->xstart)) / (screen->xstop - screen->xstart + 1);
   /* pixels_per_week = barwidth * bars_per_week */
   if (info->xscale * (7 / f1) > info->fontwidth * 5)
    i4 = 5;
   else if (info->xscale * (30 / f1) > info->fontwidth * 9)
    i4 = 4;
   else if (info->xscale * (30 / f1) > info->fontwidth * 3)
    i4 = 3;
   else
    i4 = 2;
  }
  else
   i4 = screen->datestyle;
  i2 = 13;
  buffer[0] = '\0';
  setcolor((int)screen->bgdcolor);
  rectangle(info->left, info->bottom + 2, screen->x2 - 1, info->bottom + 1 + info->fontheight);
  for (i1 = 0; i1 < screen->xwidth && screen->xstart + i1 < screen->xstop; i1++)
  {
   switch (i4)
   {
   case 2:
   case 3:
   case 4:
    taJulianToYMD(taArrayItem(info->masterbars.dt, i1 + screen->xstart), &yr, &mo, &da);
    if (i2 != mo)
    {
     if (mo == 1)
     {
      yr -= 1900;
      if (yr > 100)
       yr -= 100;
      sprintf(buffer, "%i", yr);
     }
     else
      strcpy(buffer, months[i4 - 2][mo - 1]);
     i2 = mo;
    }
    break;

   case 5:
    if (i1 + screen->xstart + 1 < info->masterbars.size &&
    taJulianDOW(taArrayItem(info->masterbars.dt, i1 + screen->xstart)) >
    taJulianDOW(taArrayItem(info->masterbars.dt, i1 + screen->xstart + 1)))
    {
     taJulianToYMD(taArrayItem(info->masterbars.dt, i1 + screen->xstart), &yr, &mo, &da);
     sprintf(buffer, "%i%s", da, months[1][mo - 1]);
    }
    break;
   }
   if (buffer[0])
   {
    i3 = info->dateleft + i1 * info->xscale;
    setcolor((int)screen->bgdcolor);
    rectangle(i3 + 1, info->bottom + 2, i3 + 1 + strlen(buffer) * info->fontwidth, info->bottom + 1 + info->fontheight);
    setcolor((int)screen->numcolor);
    moveto(i3 + 1, info->bottom + 2);
    outtext(buffer);
    setcolor((int)screen->linecolor);
    moveto(i3, info->bottom + 2);
    lineto(i3, info->bottom + 1 + info->fontheight);
    if (screen->vgrid != taNULLSTYLE && i1 > 0)
    {
     setcolor((int)screen->gridcolor);
     setlinestyle(screen->vgrid,0,0);
     moveto(i3, info->top);
     lineto(i3, info->bottom);
     setlinestyle(taSOLID,0,0);
    }
    buffer[0] = '\0';
   }
  }
  setcolor((int)screen->linecolor);
  moveto(screen->x2, info->bottom + 2);
  lineto(screen->x2, info->bottom + 1 + info->fontheight);
  if (screen->vgrid != taNULLSTYLE && screen->xstop < screen->xstart + screen->xwidth - 1)
  {
   i3 = info->dateleft + (info->xmax - screen->xstart) * info->xscale;
   setcolor((int)screen->gridcolor);
   setlinestyle(screen->vgrid,0,0);
   moveto(i3, info->top);
   lineto(i3, info->bottom);
   setlinestyle(taSOLID,0,0);
  }
 }
 for (i1 = 0; i1 < taCHART_SUBCNTMAX; i1++)
 {
  if (screen->subchartsize[i1])
  {
   for (i2 = 0, i3 = 0; i2 < screen->gridcnt; i2++)
    if (screen->chartgrid[i2].chartnum == i1)
     i3 = 1;
   if (!i3)
   {
    f2 = taChartRoundMinMax(info->subtop[i1], info->subtop[i1 + 1] - 2, info->subystart[i1], info->subystop[i1], NULL, NULL, info->fontheight);
    if (f2)
    {
     for (f1 = info->subystart[i1] + f2; f1 <= info->subystop[i1] - f2; f1 += f2)
     {
      if (screen->hgrid != taNULLSTYLE)
       taChartGrid(f1, info->subystart[i1], info->subystop[i1], info->left, info->subtop[i1], info->right, info->subtop[i1 + 1] - 2, screen->gridcolor, screen->hgrid);
      if (screen->scalepos)
      {
       setcolor((int)screen->numcolor);
       i2 = taChartCoord(f1, info->subystart[i1], info->subystop[i1], info->subtop[i1] + 1, info->subtop[i1 + 1] - 2 - 1);
       sprintf(buffer, "%6.0f", f1);
       moveto(info->scaleleft, i2 - info->fontheight / 2);
       outtext(buffer);
      }
     }
    }
   }
   if (screen->scalepos)
   {
    setcolor((int)screen->numcolor);
    sprintf(buffer, "%6.0f", info->subystart[i1]);
    moveto(info->scaleleft, info->subtop[i1 + 1] - 2 - info->fontheight);
    outtext(buffer);
    sprintf(buffer, "%6.0f", info->subystop[i1]);
    moveto(info->scaleleft, info->subtop[i1] + 1);
    outtext(buffer);
    setcolor((int)screen->linecolor);
    moveto(info->left, info->subtop[i1] - 1);
    lineto(info->right, info->subtop[i1] - 1);
   }
  }
 }
 if (screen->scalepos)
 {
  for (i1 = 0; i1 < screen->gridcnt; i1++)
  {
   setcolor((int)screen->numcolor);
   i2 = taChartCoord(screen->chartgrid[i1].value, screen->chartgrid[i1].ystart, screen->chartgrid[i1].ystop,
       info->subtop[screen->chartgrid[i1].chartnum], info->subtop[screen->chartgrid[i1].chartnum + 1] - 2);
   sprintf(buffer, "%6.0f", screen->chartgrid[i1].value);
   moveto(info->scaleleft, i2 - info->fontheight / 2);
   outtext(buffer);
  }
 }
 for (i1 = 0; i1 < screen->gridcnt; i1++)
 {
  taChartGrid(screen->chartgrid[i1].value,
      screen->chartgrid[i1].ystart, screen->chartgrid[i1].ystop,
      info->left, info->subtop[screen->chartgrid[i1].chartnum],
      info->right, info->subtop[screen->chartgrid[i1].chartnum + 1] - 2,
      screen->chartgrid[i1].color, screen->chartgrid[i1].style);
 }
 for (i1 = 0; i1 < screen->linecnt; i1++)
 {
  taChart(&screen->chartline[i1].a1, &screen->chartline[i1].b1,
      screen->xstart, screen->xstop, screen->xwidth,
      screen->chartline[i1].ystart, screen->chartline[i1].ystop,
      info->left, info->subtop[screen->chartline[i1].chartnum],
      info->right, info->subtop[screen->chartline[i1].chartnum + 1] - 2,
      screen->chartline[i1].type, screen->chartline[i1].color1,
      screen->chartline[i1].color2, screen->chartline[i1].style);
 }
 return (0);
}


int    taChartCoord(KFloat level, KFloat ystart, KFloat ystop, int y1, int y2)
{
 KFloat    y3, yscale;

 yscale = (y2 - y1) / (ystop - ystart);
 y3 = y2 - (level - ystart) * yscale;
 return ((int) y3);
}


KFloat    taChartRoundMinMax(int y1, int y2, KFloat ymin, KFloat ymax, KFloat *ystart, KFloat *ystop, int fontheight)
{
 int    i1;
 KFloat    f1, factor = 1;

 i1 = (y2 - y1) / (3 * fontheight);
 if (i1 == 0)
  return (0);
 f1 = (ymax - ymin + 1) / i1;

 while (f1 > 100)
 {
  f1 /= 10;
  factor *= 10;
 }

 if (f1 <= 2)
 {
  i1 = 2;
  f1 = 2;
 }
 else if (f1 <= 5)
 {
  i1 = 5;
  f1 = 5;
 }
 else if (f1 <= 10)
 {
  i1 = 10;
  f1 = 10;
 }
 else if (f1 <= 25)
 {
  i1 = 25;
  f1 = 25;
 }
 else
 {
  i1 = 50;
  if (f1 <= 50)
   f1 = 50;
  else
   f1 = floor((f1 + .5) / i1) * i1;
 }
 f1 *= factor;
 if (ystart)
  *ystart = floor(ymin / i1) * i1;
 if (ystop)
  *ystop = floor((ymax + i1 - .1) / i1) * i1;
 return (f1);
}


int DLL_EXPORT   taChartDraw(struct taChartScreen *screen)
{
 int    err = 0, key = 0, done = 0, redraw = 1;
 int    i1, i2;
 char     buffer[81];
 struct taChartInfo info;
 //struct videoconfig vconfig;
 //struct _fontinfo font;

 if (screen->defined != taCHART_DEFINED || !screen->linecnt)
  return (taChartErrUndefined);

#ifdef  OMMIT
 if (!setgraphmode(_VRES16COLOR))
  if (!setgraphmode(_ERESCOLOR))
   if (!setgraphmode(_HRESBW))
    return (taChartErrNoGraphics);
 //getvideoconfig(&vconfig);
#else
 //setgraphmode(0);
#endif

 memset(&info, 0, sizeof(struct taChartInfo));
 if (!screen->x2)
  screen->x2 = getmaxx();
 if (!screen->y2)
  screen->y2 = getmaxy();
#ifdef  OMMIT
// registerbgifont() Not available in WinBGI
 strcpy(buffer, taChartPath);
 strcat(buffer, "*.fon");
 if (registerbgifont(buffer) <= 0)
 {
  setgraphmode(0);
  return (taChartErrNoFonts);
 }
 if (registerbgifont("fh8b") < 0)
 {
  setgraphmode(0);
  return (taChartErrNoFonts);
 }
 _getfontinfo(&font);
 #endif
 info.fontwidth = textwidth("em")/2;
 info.fontheight = textheight("sS");

 taChartSetup(screen, &info);

 while (!done)
 {
  if (redraw)
  {
   redraw = 0;
   clearviewport();
   taChartPaint(screen, &info);
  }
#ifdef	unix
  key = toupper(getch());
#else
  key = toupper(readkey() /*getch()*/);
#endif
  switch (key)
  {
  case 'Q':
  case 27:  /* Esc */
   done = 1;
   break;

  case 'B':
   if (screen->xstart == 0)
    break;
   screen->xstart = 0;
   screen->xstop = min(info.xmax - 1, screen->xwidth - 1);
   redraw = 1;
   break;

  case 'E':
   if (screen->xstop == info.xmax - 1)
    break;
   screen->xstart = max(0, info.xmax - screen->xwidth);
   screen->xstop = info.xmax - 1;
   redraw = 1;
   break;

  case 'L':
   if (screen->xstart == 0)
    break;
   screen->xstart = max(0, screen->xstart - screen->xwidth / 2);
   screen->xstop = min(info.xmax - 1, screen->xstart + screen->xwidth - 1);
   redraw = 1;
   break;

  case 'R':
   if (screen->xstart == info.xmax - 1)
    break;
   screen->xstart = min(screen->xstart + screen->xwidth / 2, info.xmax - 2);
   screen->xstop = min(info.xmax - 1, screen->xstart + screen->xwidth - 1);
   redraw = 1;
   break;

  case 12:  /* Ctrl+L */
   if (screen->xstart == 0)
    break;
   screen->xstart--;
   screen->xstop = min(info.xmax - 1, screen->xstart + screen->xwidth - 1);
   redraw = 1;
   break;

  case 18:  /* Ctrl+R */
   if (screen->xstart == info.xmax - 2)
    break;
   screen->xstart++;
   screen->xstop = min(info.xmax - 1, screen->xstart + screen->xwidth - 1);
   redraw = 1;
   break;

  case '+':
  case '=':
   i1 = info.right - info.left + 1;
   i2 = i1 / screen->xwidth;
   if (i2 < 40)
   {
    screen->xwidth = i1 / (i2 + 1);
    screen->xstart = max(0, screen->xstop - screen->xwidth + 1);
    screen->xstop = min(info.xmax - 1, screen->xstart + screen->xwidth - 1);
    redraw = 1;
   }
   break;

  case '-':
   i1 = info.right - info.left + 1;
   i2 = i1 / screen->xwidth;
   if (i2 >= 2)
   {
    screen->xwidth = max(screen->xwidth + 1, i1 / (i2 - 1));
    screen->xstart = max(0, screen->xstop - screen->xwidth + 1);
    screen->xstop = min(info.xmax - 1, screen->xstart + screen->xwidth - 1);
    redraw = 1;
   }
   break;
  }
 }
 //setgraphmode(0);
 return (0);
}


int DLL_EXPORT   taChartPosition(struct taChartScreen *screen, struct taChartInfo *info,
			  int x1, int y1, int *barnum, int *barpos, int *subchart, KFloat *yvalue)
{
 int    i1;

 if (x1 < info->left || x1 > info->right || y1 < info->top || y1 > info->bottom)
  return (0);
 *barnum = screen->xstart + (x1 - info->dateleft) / info->xscale;
 *barpos = (int)(info->dateleft + (*barnum - screen->xstart) * info->xscale + (info->xscale / 2));
 *subchart = -1;
 *yvalue = 0;
 for (i1 = 0; i1 < taCHART_SUBCNTMAX; i1++)
 {
  if (screen->subchartsize[i1] && y1 >= info->subtop[i1] && y1 <= info->subtop[i1 + 1] - 1)
  {
   *subchart = i1;
   *yvalue = info->subystart[*subchart] + (info->subtop[*subchart + 1] - y1) * (info->subystop[*subchart] - info->subystart[*subchart]) / (info->subtop[*subchart + 1] - info->subtop[*subchart]);
   break;
  }
 }
 return (1);
}
