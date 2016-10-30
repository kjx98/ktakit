/* Technical Analysis Programmer's Toolkit - Copyright(C) 1997 KTA */

#include <stdio.h>
#include <string.h>
#include <math.h>
#include "ktakit.h"

int    taChartCoord(double level, double ystart, double ystop, int y1, int y2);
double    taChartRoundMinMax(int y1, int y2, double ymin, double ymax, double *ystart, double *ystop, int fontheight);


int DLL_EXPORT taChart(HDC hdc, taArray *pa1, taBars *pb1,
               int xstart, int xstop, int xwidth,
               double ystart, double ystop, int x1, int y1, int x2, int y2,
               int type, long color1, long color2, long style)
{
 int    err = 0, i1 = 0, pos, x3, y3, y4, xscale, halfxscale;
 double    yscale;
 int    fill[8];
 long     backcolorOld, textcolorOld;
 taArray    a1;
 taBars     b1;
 POINT    points[4];
 HPEN     hpenOld, hpen, hpen2, hpen3;
 HBRUSH   hbrushOld, hbrush, hbrush2;
 HBITMAP  hbmp;
 HRGN hrgnClip;
 RECT rcClip;

 a1 = *pa1;
 b1 = *pb1;
 if (type == taCHART_LINE && taIsNoArray(a1) && !taIsNoBars(b1))
  a1 = b1.cl;
 if (type == taCHART_HILOCL || type == taCHART_OPHILOCL || type == taCHART_CANDLE)
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
 xscale = (short) floor((x2 - x1 + 1) / xwidth);
 yscale = (y2 - y1) / (ystop - ystart);
 x1 += ((x2 - x1 + 1) - xwidth * (short)xscale) / 2;
 halfxscale = (short) floor((xscale - 2) / 2);

 GetClipBox(hdc, &rcClip);
 hrgnClip = CreateRectRgn(x1, y1, x2, y2);
 SelectClipRgn(hdc, hrgnClip);
 SetViewportOrgEx(hdc, x1, y1, NULL);

 y2 -= y1;

 if (type == taCHART_LINE)
  hpen = CreatePen((short)style, (short)color2, color1);
 else  if (type == taCHART_FILL)
  hpen = CreatePen(PS_NULL, 1, color1);
 else
  hpen = CreatePen(PS_SOLID, 1, color1);
 hpenOld = SelectObject(hdc, hpen);
 if (type == taCHART_CANDLE)
 {
  hbrush = CreateSolidBrush(color2);
  hbrush2 = CreateSolidBrush(style);
  hbrushOld = SelectObject(hdc, hbrush);
 }
 if (type == taCHART_HIST || type == taCHART_FILL)
 {
  if (style != 0 && (short)style != 0xffff)
  {
   fill[0] = (short)(style & 0xF000) >> 12;
   fill[1] = (short)(style & 0x0F00) >> 8;
   fill[2] = (short)(style & 0x00F0) >> 4;
   fill[3] = (short)(style & 0x000F);
   for (i1 = 0; i1 < 4; i1++)
    fill[i1] += (fill[i1] << 4);
   for (i1 = 4; i1 < 8; i1++)
    fill[i1] = fill[i1 - 4];
   hbmp = CreateBitmap(8, 8, 1, 1, fill);
   hbrush = CreatePatternBrush(hbmp);
   hbrushOld = SelectObject(hdc, hbrush);
   textcolorOld = GetTextColor(hdc);
   SetTextColor(hdc, color1);
   backcolorOld = GetBkColor(hdc);
   SetBkColor(hdc, color2);
  }
  else
  {
   hbrush = CreateSolidBrush(color2);
   hbrushOld = SelectObject(hdc, hbrush);
  }
 }
 if (type == taCHART_TRADES)
 {
  hpen2 = CreatePen(PS_SOLID, 1, color2);
  hpen3 = CreatePen(PS_SOLID, 1, style);
 }

 switch (type)
 {
 case taCHART_HILOCL:
 case taCHART_OPHILOCL:
  for (pos = xstart; pos <= xstop && pos < b1.size; pos++)
  {
   x3 = (pos - xstart) * xscale + halfxscale + 1;
   MoveToEx(hdc, x3, y2 - (short) ((taArrayItem(b1.hi, pos) - ystart) * yscale), NULL);
   LineTo(hdc, x3, y2 - (short) ((taArrayItem(b1.lo, pos) - ystart) * yscale) + 1);
   MoveToEx(hdc, x3 + 1, y2 - (short) ((taArrayItem(b1.cl, pos) - ystart) * yscale), NULL);
   LineTo(hdc, x3 + halfxscale + 1, y2 - (short) ((taArrayItem(b1.cl, pos) - ystart) * yscale));
   if (type == taCHART_OPHILOCL && taArrayItem(b1.op, pos))
   {
    MoveToEx(hdc, x3 - 1, y2 - (short) ((taArrayItem(b1.op, pos) - ystart) * yscale), NULL);
    LineTo(hdc, x3 - halfxscale + 1, y2 - (short) ((taArrayItem(b1.op, pos) - ystart) * yscale));
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
   x3 = (pos - xstart) * xscale + 1;
   MoveToEx(hdc, x3 + halfxscale, y2 - (short) ((taArrayItem(b1.hi, pos) - ystart) * yscale), NULL);
   LineTo(hdc, x3 + halfxscale, y2 - (short) ((taArrayItem(b1.lo, pos) - ystart) * yscale) + 1);
   if (taArrayItem(b1.op, pos) == 0 || taArrayItem(b1.op, pos) == taArrayItem(b1.cl, pos))
   {
    MoveToEx(hdc, x3, y2 - (short) ((taArrayItem(b1.cl, pos) - ystart) * yscale), NULL);
    LineTo(hdc, x3 + xscale - i1 + 1, y2 - (short) ((taArrayItem(b1.cl, pos) - ystart) * yscale));
   }
   else
   {
    if (taArrayItem(b1.op, pos) > taArrayItem(b1.cl, pos)) /* down */
    {
     SelectObject(hdc, hbrush2);
     Rectangle(hdc, x3, y2 - (short) ((taArrayItem(b1.op, pos) - ystart) * yscale),
               x3 + xscale - i1 + 1, y2 - (short) ((taArrayItem(b1.cl, pos) - ystart) * yscale) + 1);
    }
    else                                                   /*  up  */
    {
     SelectObject(hdc, hbrush);
     Rectangle(hdc, x3, y2 - (short) ((taArrayItem(b1.cl, pos) - ystart) * yscale),
               x3 + xscale - i1 + 1, y2 - (short) ((taArrayItem(b1.op, pos) - ystart) * yscale) + 1);
    }
   }
  }
  break;

 case taCHART_HIST:
  y3 = (short) (y2 + ystart * yscale);
  if (xscale == 1)
   i1 = 0;
  else
   i1 = 1;
  for (pos = xstart; pos <= xstop && pos < a1.size; pos++)
  {
   x3 = (pos - xstart) * xscale + 1;
   Rectangle(hdc, x3, y3, x3 + xscale - i1, y2 - (short)((taArrayItem(a1, pos) - ystart) * yscale) + 1);
  }
  break;

 case taCHART_LINE:
  for (pos = xstart + 1; pos <= xstop && pos < a1.size && !taArrayItem(a1, pos - 1); pos++);
  if (pos > xstop || pos == a1.size)
   break;
  MoveToEx(hdc, (pos - xstart - 1) * xscale + 1, y2 - (short) ((taArrayItem(a1, pos - 1) - ystart) * yscale), NULL);
  for (; pos <= xstop && pos < a1.size; pos++)
  {
   x3 = (pos - xstart) * xscale + halfxscale;
   LineTo(hdc, x3, y2 - (short) ((taArrayItem(a1, pos) - ystart) * yscale));
  }
  break;

 case taCHART_POINT:
  for (pos = xstart; pos <= xstop && pos < a1.size && !taArrayItem(a1, pos); pos++);
  for (; pos <= xstop && pos < a1.size; pos++)
  {
   x3 = (pos - xstart) * xscale + halfxscale + 1;
   y3 = y2 - (short) ((taArrayItem(a1, pos) - ystart) * yscale);
   MoveToEx(hdc, x3, y3, NULL);
   LineTo(hdc, x3, y3 + 1);
  }
  break;

 case taCHART_FILL:
  for (pos = xstart + 1; pos <= xstop && pos < a1.size && !taArrayItem(a1, pos - 1); pos++);
  if (pos > xstop || pos == a1.size)
   break;
  points[0].x = (pos - xstart - 1) * xscale + 1;
  points[0].y = y2 - (short) ((taArrayItem(a1, pos - 1) - ystart) * yscale);
  points[1].x = points[0].x;
  points[1].y = y2;
  points[2].y = y2;
  for (; pos <= xstop && pos < a1.size; pos++)
  {
   points[3].x = (pos - xstart) * xscale + halfxscale;
   points[3].y = y2 - (short) ((taArrayItem(a1, pos) - ystart) * yscale);
   points[2].x = points[3].x;
   Polygon(hdc, points, 4);
   points[0] = points[3];
   points[1] = points[2];
  }
  break;

 case taCHART_TRADES:
  for (pos = xstart; pos <= xstop && pos < a1.size; pos++)
  {
   i1 = (short)taArrayItem(a1, pos);
   if (i1 == 0)
    continue;
   x3 = (pos - xstart) * xscale + halfxscale + 1;
   y3 = y2 - (short)((taArrayItem(b1.hi, pos) - ystart) * yscale + 2);
   y4 = y2 - (short)((taArrayItem(b1.lo, pos) - ystart) * yscale - 2);
   if (i1 & taBO)
   {
    SelectObject(hdc, hpen);
    MoveToEx(hdc, x3, y4 + xscale, NULL);
    LineTo(hdc, x3, y4);
    LineTo(hdc, x3 + halfxscale, y4 + halfxscale);
    MoveToEx(hdc, x3, y4, NULL);
    LineTo(hdc, x3 - halfxscale, y4 + halfxscale);
   }
   if (i1 & taBC)
   {
    SelectObject(hdc, hpen3);
    MoveToEx(hdc, x3, y4 + xscale * 2, NULL);
    LineTo(hdc, x3, y4 + xscale);
    LineTo(hdc, x3 + halfxscale, y4 + xscale + halfxscale);
    MoveToEx(hdc, x3, y4 + xscale, NULL);
    LineTo(hdc, x3 - halfxscale, y4 + xscale + halfxscale);
    MoveToEx(hdc, x3 - halfxscale, y4 + xscale * 2, NULL);
    LineTo(hdc, x3 + halfxscale, y4 + xscale * 2);
   }
   if (i1 & taSO)
   {
    SelectObject(hdc, hpen2);
    MoveToEx(hdc, x3, y3 - xscale, NULL);
    LineTo(hdc, x3, y3);
    LineTo(hdc, x3 + halfxscale, y3 - halfxscale);
    MoveToEx(hdc, x3, y3, NULL);
    LineTo(hdc, x3 - halfxscale, y3 - halfxscale);
   }
   if (i1 & taSC)
   {
    SelectObject(hdc, hpen3);
    MoveToEx(hdc, x3, y3 - xscale * 2, NULL);
    LineTo(hdc, x3, y3 - xscale);
    LineTo(hdc, x3 + halfxscale, y3 - xscale - halfxscale);
    MoveToEx(hdc, x3, y3 - xscale, NULL);
    LineTo(hdc, x3 - halfxscale, y3 - xscale - halfxscale);
    MoveToEx(hdc, x3 - halfxscale, y3 - xscale * 2, NULL);
    LineTo(hdc, x3 + halfxscale, y3 - xscale * 2);
   }
  }
  break;

 default:
  err = taErrInvalidParm;
  break;
 }

 DeleteObject(hrgnClip);
 hrgnClip = CreateRectRgn(rcClip.left, rcClip.top, rcClip.right, rcClip.bottom);
 SelectClipRgn(hdc, hrgnClip);
 DeleteObject(hrgnClip);
 SetViewportOrgEx(hdc, 0, 0, NULL);

 SelectObject(hdc, hpenOld);
 DeleteObject(hpen);
 if (type == taCHART_CANDLE)
 {
  SelectObject(hdc, hbrushOld);
  DeleteObject(hbrush);
  DeleteObject(hbrush2);
 }
 if (type == taCHART_HIST || type == taCHART_FILL)
 {
  SelectObject(hdc, hbrushOld);
  DeleteObject(hbrush);
  if (style != 0 && (short)style != 0xffff)
  {
   DeleteObject(hbmp);
   SetTextColor(hdc, textcolorOld);
   SetBkColor(hdc, backcolorOld);
  }
 }
 if (type == taCHART_TRADES)
 {
  DeleteObject(hpen2);
  DeleteObject(hpen3);
 }
 return (err);
}


int DLL_EXPORT taChartGrid(HDC hdc, double level, double ystart, double ystop,
               int x1, int y1, int x2, int y2, long color, long style)
{
 double    y3, yscale;
 HPEN     hpenOld, hpen;

 yscale = (y2 - y1) / (ystop - ystart);
 y3 = y2 - (level - ystart) * yscale;
 if (y3 > y2 || y3 < y1)
  return (0);
 hpen = CreatePen((short)style, 1, color);
 hpenOld = SelectObject(hdc, hpen);
 MoveToEx(hdc, x1, (short) y3, NULL);
 LineTo(hdc, x2, (short) y3);
 SelectObject(hdc, hpenOld);
 DeleteObject(hpen);
 return (0);
}


int DLL_EXPORT taChartScreenInit(struct taChartScreen *screen, int linemax, int gridmax)
{
 memset(screen, 0, sizeof(struct taChartScreen));
 screen->chartline = taMalloc(linemax * sizeof(struct taChartLine));
 if (screen->chartline == NULL)
  return (taErrMalloc);
 screen->chartgrid = taMalloc(gridmax * sizeof(struct taChartGrids));
 if (screen->chartgrid == NULL)
 {
  taFree(screen->chartline);
  return (taErrMalloc);
 }
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
 screen->linemax = linemax;
 screen->gridmax = gridmax;
 return(0);
}


void DLL_EXPORT taChartScreenClose(struct taChartScreen *screen)
{
 taFree(screen->chartline);
 taFree(screen->chartgrid);
 return;
}


int DLL_EXPORT taChartLineDef(struct taChartScreen *screen, int chartnum,
               taArray *a1, taBars *b1, double ystart, double ystop,
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
               int chartnum, double value, double ystart, double ystop,
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

int DLL_EXPORT taChartSetup(HDC hdc, struct taChartScreen *screen, struct taChartInfo *info)
{
 int    i1, i2, i3;
 long     backcolorOld, textcolorOld;
 int    mapmodeOld;
 HPEN     hpenOld, hpen;
 HBRUSH   hbrushOld, hbrush;
 HFONT    hfontOld, hfont;
 SIZE     size1;

 if (screen->defined != taCHART_DEFINED || !screen->linecnt)
  return (taChartErrUndefined);
 memset(info, 0, sizeof(struct taChartInfo));

 mapmodeOld = SetMapMode(hdc, MM_TEXT);
 hpen = CreatePen(PS_SOLID, 1, screen->linecolor);
 hpenOld = SelectObject(hdc, hpen);
 hbrush = CreateSolidBrush(screen->bgdcolor);
 hbrushOld = SelectObject(hdc, hbrush);
 textcolorOld = GetTextColor(hdc);
 SetTextColor(hdc, screen->fgdcolor);
 backcolorOld = GetBkColor(hdc);
 SetBkColor(hdc, screen->bgdcolor);
 i1 = -MulDiv(8, GetDeviceCaps(hdc, LOGPIXELSY), 72);
 hfont = CreateFont(i1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, FF_MODERN + FIXED_PITCH, "");
 hfontOld = SelectObject(hdc, hfont);

 GetTextExtentPoint(hdc, "Yy", 2, &size1);
 info->fontwidth = size1.cx / 2;
 info->fontheight = size1.cy;

 Rectangle(hdc, screen->x1, screen->y1, screen->x2 + 1, screen->y2 + 1);

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

 screen->heading[159] = '\0';
 taTrim(screen->heading);
 screen->footing[159] = '\0';
 taTrim(screen->footing);
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
  TextOut(hdc, screen->x1 + i1, info->top, screen->heading + i2, strlen(screen->heading + i2));
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
  TextOut(hdc, screen->x1 + i1, info->bottom + 2, screen->footing + i2, strlen(screen->footing + i2));
  MoveToEx(hdc, screen->x1, info->bottom + 1, NULL);
  LineTo(hdc, screen->x2, info->bottom + 1);
 }
 if (screen->datestyle)
  info->bottom -= info->fontheight + 1;

 MoveToEx(hdc, screen->x1, info->top - 1, NULL);
 LineTo(hdc, screen->x2, info->top - 1);
 MoveToEx(hdc, screen->x1, info->bottom + 1, NULL);
 LineTo(hdc, screen->x2, info->bottom + 1);
 if (screen->scalepos)
 {
  MoveToEx(hdc, info->scaleleft - 1, info->top, NULL);
  LineTo(hdc, info->scaleleft - 1, info->bottom + 1);
  MoveToEx(hdc, info->scaleright - 1, info->top, NULL);
  LineTo(hdc, info->scaleright - 1, info->bottom + 1);
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
   MoveToEx(hdc, screen->x1, info->subtop[i1], NULL);
   LineTo(hdc, screen->x2, info->subtop[i1]);
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
   {
    taBarStats(&screen->chartline[i1].b1, &screen->chartline[i1].ystart, &screen->chartline[i1].ystop, 0, screen->chartline[i1].b1.size - 1);
   }
   else
   {
    taArrayStats(&screen->chartline[i1].a1, &screen->chartline[i1].ystart, &screen->chartline[i1].ystop, NULL, NULL, 0, screen->chartline[i1].a1.size - 1, 0);
   }
  }
  if (screen->chartline[i1].ystart != screen->chartline[i1].ystop)
  {
   taChartRoundMinMax(info->subtop[screen->chartline[i1].chartnum],
     info->subtop[screen->chartline[i1].chartnum + 1] - 2,
     screen->chartline[i1].ystart, screen->chartline[i1].ystop,
     &screen->chartline[i1].ystart, &screen->chartline[i1].ystop,
     info->fontheight);
  }
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
  {
   info->xmax = max(info->xmax, screen->chartline[i1].a1.size);
  }
 }
 if (!taIsNoBars(info->masterbars))
  info->xmax = min(info->xmax, info->masterbars.size);

 if (screen->xwidth == 0)
  screen->xwidth = (info->right - info->left + 1) / 4;
 screen->xstop = info->xmax - 1;
 screen->xstart = max(0, screen->xstop - screen->xwidth + 1);

 SelectObject(hdc, hpenOld);
 SelectObject(hdc, hbrushOld);
 SelectObject(hdc, hfontOld);
 SetTextColor(hdc, textcolorOld);
 SetBkColor(hdc, backcolorOld);
 DeleteObject(hpen);
 DeleteObject(hbrush);
 DeleteObject(hfont);
 SetMapMode(hdc, mapmodeOld);
 return (0);
}


int DLL_EXPORT taChartPaint(HDC hdc, struct taChartScreen *screen, struct taChartInfo *info)
{
 int    i1, i2, i3, i4;
 int    yr, mo, da;
 double    f1, f2;
 long     backcolorOld, textcolorOld;
 int    mapmodeOld;
 char     buffer[81];
 char    *months[3][12] = {
  {"J", "F", "M", "A", "M", "J", "J", "A", "S", "O", "N", "D"},
  {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"},
  {"January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December"},
 };
 HPEN     hpenOld, hpen, hpenv, hpenh, hpennull;
 HBRUSH   hbrushOld, hbrush;
 HFONT    hfontOld, hfont;

 mapmodeOld = SetMapMode(hdc, MM_TEXT);
 hpen = CreatePen(PS_SOLID, 1, screen->linecolor);
 hpenv = CreatePen(screen->vgrid, 1, screen->gridcolor);
 hpenh = CreatePen(screen->hgrid, 1, screen->gridcolor);
 hpennull = CreatePen(PS_NULL, 1, 0);
 hpenOld = SelectObject(hdc, hpen);
 hbrush = CreateSolidBrush(screen->bgdcolor);
 hbrushOld = SelectObject(hdc, hbrush);
 textcolorOld = GetTextColor(hdc);
 SetTextColor(hdc, screen->numcolor);
 backcolorOld = GetBkColor(hdc);
 SetBkColor(hdc, screen->bgdcolor);
 i1 = -MulDiv(8, GetDeviceCaps(hdc, LOGPIXELSY), 72);
 hfont = CreateFont(i1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, FF_MODERN + FIXED_PITCH, "");
 hfontOld = SelectObject(hdc, hfont);

 SelectObject(hdc, hpennull);
 Rectangle(hdc, info->left, info->top, info->right + 2, info->bottom + 2);

 i1 = info->right - info->left + 1;
 info->xscale = (short) floor(i1 / screen->xwidth);
 info->dateleft = info->left + (i1 - screen->xwidth * (short) info->xscale) / 2;
 if (screen->datestyle && !taIsNoBars(info->masterbars) && (taArrayItem(info->masterbars.dt, screen->xstop) - taArrayItem(info->masterbars.dt, screen->xstart)) >= 1)
 {
  if (screen->datestyle == 1)
  {
   f1 = (float) (taArrayItem(info->masterbars.dt, screen->xstop) - taArrayItem(info->masterbars.dt, screen->xstart)) / (screen->xstop - screen->xstart + 1);
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
  SelectObject(hdc, hpennull);
  Rectangle(hdc, info->left, info->bottom + 2, screen->x2, info->bottom + 3 + info->fontheight);
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
      if (yr >= 100)
       yr -= 100;
      sprintf(buffer, "%2.2i", yr);
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
    SelectObject(hdc, hpennull);
    Rectangle(hdc, i3 + 1, info->bottom + 2, i3 + 1 + strlen(buffer) * info->fontwidth, info->bottom + 1 + info->fontheight);
    TextOut(hdc, i3 + 1, info->bottom + 2, buffer, strlen(buffer));
    SelectObject(hdc, hpen);
    MoveToEx(hdc, i3, info->bottom + 2, NULL);
    LineTo(hdc, i3, info->bottom + 2 + info->fontheight);
    if (screen->vgrid != taNULLSTYLE && i1 > 0)
    {
     SelectObject(hdc, hpenv);
     MoveToEx(hdc, i3, info->top, NULL);
     LineTo(hdc, i3, info->bottom);
    }
    buffer[0] = '\0';
   }
  }
 }
 SelectObject(hdc, hpen);
 MoveToEx(hdc, screen->x2, info->bottom + 2, NULL);
 LineTo(hdc, screen->x2, info->bottom + 1 + info->fontheight);
 if (screen->vgrid != taNULLSTYLE && screen->xstop < screen->xstart + screen->xwidth - 1)
 {
  i3 = info->dateleft + (info->xmax - screen->xstart) * info->xscale;
  SelectObject(hdc, hpenv);
  MoveToEx(hdc, i3, info->top, NULL);
  LineTo(hdc, i3, info->bottom);
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
       taChartGrid(hdc, f1, info->subystart[i1], info->subystop[i1], info->left, info->subtop[i1], info->right, info->subtop[i1 + 1] - 2, screen->gridcolor, screen->hgrid);
      if (screen->scalepos)
      {
       i2 = taChartCoord(f1, info->subystart[i1], info->subystop[i1], info->subtop[i1] + 1, info->subtop[i1 + 1] - 2 - 1);
       sprintf(buffer, "%6li", (long)f1);
       TextOut(hdc, info->scaleleft, i2 - info->fontheight / 2, buffer, strlen(buffer));
      }
     }
    }
   }
   if (screen->scalepos)
   {
    sprintf(buffer, "%6li", (long)info->subystart[i1]);
    TextOut(hdc, info->scaleleft, info->subtop[i1 + 1] - 2 - info->fontheight, buffer, strlen(buffer));
    sprintf(buffer, "%6li", (long)info->subystop[i1]);
    TextOut(hdc, info->scaleleft, info->subtop[i1] + 1, buffer, strlen(buffer));
    SelectObject(hdc, hpen);
    MoveToEx(hdc, info->left, info->subtop[i1] - 1, NULL);
    LineTo(hdc, info->right, info->subtop[i1] - 1);
   }
  }
 }
 if (screen->scalepos)
 {
  for (i1 = 0; i1 < screen->gridcnt; i1++)
  {
   i2 = taChartCoord(screen->chartgrid[i1].value, screen->chartgrid[i1].ystart, screen->chartgrid[i1].ystop,
       info->subtop[screen->chartgrid[i1].chartnum], info->subtop[screen->chartgrid[i1].chartnum + 1] - 2);
   sprintf(buffer, "%6li", (long)screen->chartgrid[i1].value);
   TextOut(hdc, info->scaleleft, i2 - info->fontheight / 2, buffer, strlen(buffer));
  }
 }
 for (i1 = 0; i1 < screen->gridcnt; i1++)
 {
  taChartGrid(hdc, screen->chartgrid[i1].value,
      screen->chartgrid[i1].ystart, screen->chartgrid[i1].ystop,
      info->left, info->subtop[screen->chartgrid[i1].chartnum],
      info->right, info->subtop[screen->chartgrid[i1].chartnum + 1] - 2,
      screen->chartgrid[i1].color, screen->chartgrid[i1].style);
 }

 for (i1 = 0; i1 < screen->linecnt; i1++)
 {
  taChart(hdc, &screen->chartline[i1].a1, &screen->chartline[i1].b1,
      screen->xstart, screen->xstop, screen->xwidth,
      screen->chartline[i1].ystart, screen->chartline[i1].ystop,
      info->left, info->subtop[screen->chartline[i1].chartnum],
      info->right, info->subtop[screen->chartline[i1].chartnum + 1] - 2,
      screen->chartline[i1].type, screen->chartline[i1].color1,
      screen->chartline[i1].color2, screen->chartline[i1].style);
 }
 SelectObject(hdc, hpenOld);
 SelectObject(hdc, hbrushOld);
 SelectObject(hdc, hfontOld);
 SetTextColor(hdc, textcolorOld);
 SetBkColor(hdc, backcolorOld);
 DeleteObject(hpen);
 DeleteObject(hpenv);
 DeleteObject(hpenh);
 DeleteObject(hpennull);
 DeleteObject(hbrush);
 DeleteObject(hfont);
 SetMapMode(hdc, mapmodeOld);
 return (0);
}


int      taChartCoord(double level, double ystart, double ystop, int y1, int y2)
{
 double    y3, yscale;

 yscale = (y2 - y1) / (ystop - ystart);
 y3 = y2 - (level - ystart) * yscale;
 return ((int) y3);
}


double    taChartRoundMinMax(int y1, int y2, double ymin, double ymax, double *ystart, double *ystop, int fontheight)
{
 int      i1;
 double    f1, factor = 1;

 i1 = (y2 - y1) / (2 * fontheight);
 if (i1 == 0)
  return (0);
 f1 = (ymax - ymin + 1) / i1;

 while (f1 > 100)
 {
  f1 /= 10;
  factor *= 10;
 }

 if (f1 <= 1)
 {
  i1 = 1;
  f1 = 1;
 }
 else if (f1 <= 2)
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


int DLL_EXPORT taChartPosition(struct taChartScreen *screen, struct taChartInfo *info,
			  int x1, int y1, int *barnum, int *barpos, int *subchart, double *yvalue)
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
