/* Technical Analysis Programmer's Toolkit - Copyright (C) 1997 KTA */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "ktakit.h"


int DLL_EXPORT taCandleConstruct(taBars *b1, struct taCandleInfo *info)
{
 int    pos;
 double    len, realbodytop, realbodybot;
 double   acr = 0; /* AverageCandleRange */

 if((info->candle = (struct taCandles *)taMalloc(b1->size * sizeof(struct taCandles))) == NULL)
  return(taErrMalloc);

 for (pos = 0; pos < b1->size; pos++)
  acr += fabs(taArrayItem(b1->op, pos) - taArrayItem(b1->cl, pos));
 acr /= b1->size;
 for (pos = 0; pos < b1->size; pos++)
 {
  if (taArrayItem(b1->op, pos) > taArrayItem(b1->cl, pos))
   info->candle[pos].color = taBLACKCNDL;
  else if (taArrayItem(b1->op, pos) < taArrayItem(b1->cl, pos))
   info->candle[pos].color = taWHITECNDL;
  else
   info->candle[pos].color = taNONE;
  len = fabs(taArrayItem(b1->op, pos) - taArrayItem(b1->cl, pos));
  realbodybot = min(taArrayItem(b1->op, pos), taArrayItem(b1->cl, pos));
  realbodytop = max(taArrayItem(b1->op, pos), taArrayItem(b1->cl, pos));
  if (len > acr)
   info->candle[pos].shape = taLARGE;
  else if (len > realbodybot * info->equalpct)
   info->candle[pos].shape = taSMALL;
  else
   info->candle[pos].shape = taDOJI;
  if (info->candle[pos].color == taNONE)
     len = info->equalpct * realbodytop;
  if (taArrayItem(b1->hi, pos) - realbodytop > len * info->largeshadow)
   info->candle[pos].topshadow = taLARGE;
  else if (taArrayItem(b1->hi, pos) - realbodytop < len * info->smallshadow)
   info->candle[pos].topshadow = taSMALL;
  else
   info->candle[pos].topshadow = taMEDIUM;
  if (realbodybot - taArrayItem(b1->lo, pos) > len * info->largeshadow)
   info->candle[pos].botshadow = taLARGE;
  else if (realbodybot - taArrayItem(b1->lo, pos) < len * info->smallshadow)
   info->candle[pos].botshadow = taSMALL;
  else
   info->candle[pos].botshadow = taMEDIUM;
 }
 return (0);
}

void DLL_EXPORT taCandleFree(struct taCandleInfo *info)
{
 taFree(info->candle);
 return;
}

int DLL_EXPORT taCandleFill(taBars *b1, struct taCandleInfo *info, taArray *a2,
                        char *candlefunct, int start, int stop)
{
 int    i1, pos, cnt = 0;
 char    *formations[] =
 {
  "taAbandonedBabyBottom" ,
  "taAbandonedBabyTop"    ,
  "taAdvanceBlock"        ,
  "taBeltHoldLineBear"    ,
  "taBeltHoldLineBull"    ,
  "taCounterAttackBear"   ,
  "taCounterAttackBull"   ,
  "taDarkCloudCover"      ,
  "taEngulfingLineBear"   ,
  "taEngulfingLineBull"   ,
  "taEveningDojiStar"     ,
  "taEveningStar"         ,
  "taFalling3Method"      ,
  "taGravestoneDoji"      ,
  "taHammer"              ,
  "taHangingman"          ,
  "taHaramiBlack"         ,
  "taHaramiCrossBlack"    ,
  "taHaramiCrossWhite"    ,
  "taHaramiWhite"         ,
  "taHighWave"            ,
  "taInNeckLine"          ,
  "taInvertedHammer"      ,
  "taLongLeggedDoji"      ,
  "taMatHoldPattern"      ,
  "taMorningDojiStar"     ,
  "taMorningStar"         ,
  "taPiercingLine"        ,
  "taRising3Method"       ,
  "taSeperatingLineBear"  ,
  "taSeperatingLineBull"  ,
  "taShootingStar"        ,
  "taSideBySideWhiteGapDn",
  "taSideBySideWhiteGapUp",
  "taStalledPattern"      ,
  "taTasukiDownsideGap"   ,
  "taTasukiUpsideGap"     ,
  "taThreeGapsUp"         ,
  "taThreeGapsDown"       ,
  "taThreeWhiteSoldiers"  ,
  "taThrustingLine"       ,
  "taUpsideGap2Crows"     ,
  ""};

 stop = min(stop, min(b1->size - 1, a2->size - 1));
 for (i1 = 0; formations[i1][0]; i1++)
  if (!stricmp(candlefunct, formations[i1]))
   break;
 switch (i1)
 {
 case 0:
  for(pos = start; pos <= stop; pos++)
   cnt += ((taArrayItemP(a2, pos) = taAbandonedBabyBottom(b1, info, pos)) != taFALSE);
  break;
 case 1:
  for(pos = start; pos <= stop; pos++)
   cnt += ((taArrayItemP(a2, pos) = taAbandonedBabyTop(b1, info, pos)) != taFALSE);
  break;
 case 2:
  for(pos = start; pos <= stop; pos++)
   cnt += ((taArrayItemP(a2, pos) = taAdvanceBlock(b1, info, pos)) != taFALSE);
  break;
 case 3:
  for(pos = start; pos <= stop; pos++)
   cnt += ((taArrayItemP(a2, pos) = taBeltHoldLineBear(b1, info, pos)) != taFALSE);
  break;
 case 4:
  for(pos = start; pos <= stop; pos++)
   cnt += ((taArrayItemP(a2, pos) = taBeltHoldLineBull(b1, info, pos)) != taFALSE);
  break;
 case 5:
  for(pos = start; pos <= stop; pos++)
   cnt += ((taArrayItemP(a2, pos) = taCounterAttackBear(b1, info, pos)) != taFALSE);
  break;
 case 6:
  for(pos = start; pos <= stop; pos++)
   cnt += ((taArrayItemP(a2, pos) = taCounterAttackBull(b1, info, pos)) != taFALSE);
  break;
 case 7:
  for(pos = start; pos <= stop; pos++)
   cnt += ((taArrayItemP(a2, pos) = taDarkCloudCover(b1, info, pos)) != taFALSE);
  break;
 case 8:
  for(pos = start; pos <= stop; pos++)
   cnt += ((taArrayItemP(a2, pos) = taEngulfingLineBear(b1, info, pos)) != taFALSE);
  break;
 case 9:
  for(pos = start; pos <= stop; pos++)
   cnt += ((taArrayItemP(a2, pos) = taEngulfingLineBull(b1, info, pos)) != taFALSE);
  break;
 case 10:
  for(pos = start; pos <= stop; pos++)
   cnt += ((taArrayItemP(a2, pos) = taEveningDojiStar(b1, info, pos)) != taFALSE);
  break;
 case 11:
  for(pos = start; pos <= stop; pos++)
   cnt += ((taArrayItemP(a2, pos) = taEveningStar(b1, info, pos)) != taFALSE);
  break;
 case 12:
  for(pos = start; pos <= stop; pos++)
   cnt += ((taArrayItemP(a2, pos) = taFalling3Method(b1, info, pos)) != taFALSE);
  break;
 case 13:
  for(pos = start; pos <= stop; pos++)
   cnt += ((taArrayItemP(a2, pos) = taGravestoneDoji(b1, info, pos)) != taFALSE);
  break;
 case 14:
  for(pos = start; pos <= stop; pos++)
   cnt += ((taArrayItemP(a2, pos) = taHammer(b1, info, pos)) != taFALSE);
  break;
 case 15:
  for(pos = start; pos <= stop; pos++)
   cnt += ((taArrayItemP(a2, pos) = taHangingman(b1, info, pos)) != taFALSE);
  break;
 case 16:
  for(pos = start; pos <= stop; pos++)
   cnt += ((taArrayItemP(a2, pos) = taHaramiBlack(b1, info, pos)) != taFALSE);
  break;
 case 17:
  for(pos = start; pos <= stop; pos++)
   cnt += ((taArrayItemP(a2, pos) = taHaramiCrossBlack(b1, info, pos)) != taFALSE);
  break;
 case 18:
  for(pos = start; pos <= stop; pos++)
   cnt += ((taArrayItemP(a2, pos) = taHaramiCrossWhite(b1, info, pos)) != taFALSE);
  break;
 case 19:
  for(pos = start; pos <= stop; pos++)
   cnt += ((taArrayItemP(a2, pos) = taHaramiWhite(b1, info, pos)) != taFALSE);
  break;
 case 20:
  for(pos = start; pos <= stop; pos++)
   cnt += ((taArrayItemP(a2, pos) = taHighWave(b1, info, pos)) != taFALSE);
  break;
 case 21:
  for(pos = start; pos <= stop; pos++)
   cnt += ((taArrayItemP(a2, pos) = taInNeckLine(b1, info, pos)) != taFALSE);
  break;
 case 22:
  for(pos = start; pos <= stop; pos++)
   cnt += ((taArrayItemP(a2, pos) = taInvertedHammer(b1, info, pos)) != taFALSE);
  break;
 case 23:
  for(pos = start; pos <= stop; pos++)
   cnt += ((taArrayItemP(a2, pos) = taLongLeggedDoji(b1, info, pos)) != taFALSE);
  break;
 case 24:
  for(pos = start; pos <= stop; pos++)
   cnt += ((taArrayItemP(a2, pos) = taMatHoldPattern(b1, info, pos)) != taFALSE);
  break;
 case 25:
  for(pos = start; pos <= stop; pos++)
   cnt += ((taArrayItemP(a2, pos) = taMorningDojiStar(b1, info, pos)) != taFALSE);
  break;
 case 26:
  for(pos = start; pos <= stop; pos++)
   cnt += ((taArrayItemP(a2, pos) = taMorningStar(b1, info, pos)) != taFALSE);
  break;
 case 27:
  for(pos = start; pos <= stop; pos++)
   cnt += ((taArrayItemP(a2, pos) = taPiercingLine(b1, info, pos)) != taFALSE);
  break;
 case 28:
  for(pos = start; pos <= stop; pos++)
   cnt += ((taArrayItemP(a2, pos) = taRising3Method(b1, info, pos)) != taFALSE);
  break;
 case 29:
  for(pos = start; pos <= stop; pos++)
   cnt += ((taArrayItemP(a2, pos) = taSeperatingLineBear(b1, info, pos)) != taFALSE);
  break;
 case 30:
  for(pos = start; pos <= stop; pos++)
   cnt += ((taArrayItemP(a2, pos) = taSeperatingLineBull(b1, info, pos)) != taFALSE);
  break;
 case 31:
  for(pos = start; pos <= stop; pos++)
   cnt += ((taArrayItemP(a2, pos) = taShootingStar(b1, info, pos)) != taFALSE);
  break;
 case 32:
  for(pos = start; pos <= stop; pos++)
   cnt += ((taArrayItemP(a2, pos) = taSideBySideWhiteGapDn(b1, info, pos)) != taFALSE);
  break;
 case 33:
  for(pos = start; pos <= stop; pos++)
   cnt += ((taArrayItemP(a2, pos) = taSideBySideWhiteGapUp(b1, info, pos)) != taFALSE);
  break;
 case 34:
  for(pos = start; pos <= stop; pos++)
   cnt += ((taArrayItemP(a2, pos) = taStalledPattern(b1, info, pos)) != taFALSE);
  break;
 case 35:
  for(pos = start; pos <= stop; pos++)
   cnt += ((taArrayItemP(a2, pos) = taTasukiDownsideGap(b1, info, pos)) != taFALSE);
  break;
 case 36:
  for(pos = start; pos <= stop; pos++)
   cnt += ((taArrayItemP(a2, pos) = taTasukiUpsideGap(b1, info, pos)) != taFALSE);
  break;
 case 37:
  for(pos = start; pos <= stop; pos++)
   cnt += ((taArrayItemP(a2, pos) = taThreeGapsUp(b1, info, pos)) != taFALSE);
  break;
 case 38:
  for(pos = start; pos <= stop; pos++)
   cnt += ((taArrayItemP(a2, pos) = taThreeGapsDown(b1, info, pos)) != taFALSE);
  break;
 case 39:
  for(pos = start; pos <= stop; pos++)
   cnt += ((taArrayItemP(a2, pos) = taThreeWhiteSoldiers(b1, info, pos)) != taFALSE);
  break;
 case 40:
  for(pos = start; pos <= stop; pos++)
   cnt += ((taArrayItemP(a2, pos) = taThrustingLine(b1, info, pos)) != taFALSE);
  break;
 case 41:
  for(pos = start; pos <= stop; pos++)
   cnt += ((taArrayItemP(a2, pos) = taUpsideGap2Crows(b1, info, pos)) != taFALSE);
  break;
 default:
  return (taErrInvalidParm);
 }
 return (cnt);
}

int DLL_EXPORT taApproxEqual(double f1, double f2, double pct)
{
 return (fabs(f1 - f2) < f1 * pct);
}

double DLL_EXPORT taBodyMidpoint(taBars *b1, int pos)
{
 return ((taArrayItem(b1->op, pos) + taArrayItem(b1->cl, pos)) / 2);
}

int DLL_EXPORT taBodyGapUp(taBars *b1, int pos)
{
 return (min(taArrayItem(b1->op, pos), taArrayItem(b1->cl, pos)) >
         max(taArrayItem(b1->op, (pos) - 1), taArrayItem(b1->cl, (pos - 1))));
}

int DLL_EXPORT taBodyGapDn(taBars *b1, int pos)
{
 return (max(taArrayItem(b1->op, pos), taArrayItem(b1->cl, pos)) <
         min(taArrayItem(b1->op, (pos) - 1), taArrayItem(b1->cl, (pos - 1))));
}

int DLL_EXPORT taWesternGapUp(taBars *b1, int pos)
{
 return (taArrayItem(b1->lo, pos) > taArrayItem(b1->hi, (pos - 1)));
}

int DLL_EXPORT taWesternGapDn(taBars *b1, int pos)
{
 return (taArrayItem(b1->hi, pos) < taArrayItem(b1->lo, (pos - 1)));
}



int DLL_EXPORT taAbandonedBabyBottom(taBars *b1, struct taCandleInfo *info, int pos)
{
 if (pos < 2)
  return (taFALSE);
 if (info->candle[pos].color == taWHITECNDL &&
     info->candle[pos - 2].color == taBLACKCNDL &&
     info->candle[pos - 1].shape == taDOJI &&
     info->candle[pos - 2].shape == taLARGE &&
     taWesternGapDn(b1, pos - 1) &&
     taWesternGapUp(b1, pos) &&
     taArrayItem(b1->cl, pos) > taBodyMidpoint(b1, pos - 2))
  return (taTRUE);
 else
  return (taFALSE);
}


int DLL_EXPORT taAbandonedBabyTop(taBars *b1, struct taCandleInfo *info, int pos)
{
 if (pos < 2)
  return (taFALSE);
 if (info->candle[pos].color == taBLACKCNDL &&
     info->candle[pos - 2].color == taWHITECNDL &&
     info->candle[pos - 1].shape == taDOJI &&
     info->candle[pos - 2].shape == taLARGE &&
     taWesternGapUp(b1, pos - 1) &&
     taWesternGapDn(b1, pos) &&
     taArrayItem(b1->cl, pos) < taBodyMidpoint(b1, pos - 2))
  return (taTRUE);
 else
  return (taFALSE);
}

int DLL_EXPORT taAdvanceBlock(taBars *b1, struct taCandleInfo *info, int pos)
{
 double    len0, len1, len2;

 if (pos < 2)
  return (taFALSE);
 len0 = taArrayItem(b1->cl, pos) - taArrayItem(b1->op, pos);
 len1 = taArrayItem(b1->cl, pos - 1) - taArrayItem(b1->op, pos - 1);
 len2 = taArrayItem(b1->cl, pos - 2) - taArrayItem(b1->op, pos - 2);

 if (info->candle[pos].color == taWHITECNDL &&
     info->candle[pos - 1].color == taWHITECNDL &&
     info->candle[pos - 2].color == taWHITECNDL &&
     taArrayItem(b1->cl, pos) > taArrayItem(b1->cl, pos - 1) &&
     taArrayItem(b1->cl, pos - 1) > taArrayItem(b1->cl, pos - 2) &&
     ((len2 > len1 && len1 > len0) ||
      (info->candle[pos - 2].topshadow != taLARGE &&
       info->candle[pos].topshadow == taLARGE && info->candle[pos - 1].topshadow == taLARGE)))
  return (taTRUE);
 else
  return (taFALSE);
}

int DLL_EXPORT taBeltHoldLineBull(taBars *b1, struct taCandleInfo *info, int pos)
{
 if (info->candle[pos].color == taWHITECNDL &&
     info->candle[pos].shape == taLARGE &&
     taArrayItem(b1->op, pos) == taArrayItem(b1->lo, pos))
  return (taTRUE);
 else
  return (taFALSE);
}

int DLL_EXPORT taBeltHoldLineBear(taBars *b1, struct taCandleInfo *info, int pos)
{
 if (info->candle[pos].color == taBLACKCNDL &&
     info->candle[pos].shape == taLARGE &&
     taArrayItem(b1->op, pos) == taArrayItem(b1->hi, pos))
  return (taTRUE);
 else
  return (taFALSE);
}

int DLL_EXPORT taCounterAttackBear(taBars *b1, struct taCandleInfo *info, int pos)
{
 if (pos < 1)
  return (taFALSE);
 if (info->candle[pos].color == taBLACKCNDL &&
     info->candle[pos - 1].color == taWHITECNDL &&
     info->candle[pos].shape != taDOJI &&
     info->candle[pos - 1].shape != taDOJI &&
     taApproxEqual(taArrayItem(b1->cl, pos), taArrayItem(b1->cl, pos - 1), info->equalpct) &&
     taArrayItem(b1->op, pos) - taArrayItem(b1->cl, pos - 1) >=
        taArrayItem(b1->cl, pos - 1) - taArrayItem(b1->op, pos - 1))
  return (taTRUE);
 else
  return (taFALSE);
}

int DLL_EXPORT taCounterAttackBull(taBars *b1, struct taCandleInfo *info, int pos)
{
 if (pos < 1)
  return (taFALSE);
 if (info->candle[pos].color == taWHITECNDL &&
     info->candle[pos - 1].color == taBLACKCNDL &&
     info->candle[pos].shape != taDOJI &&
     info->candle[pos - 1].shape != taDOJI &&
     taApproxEqual(taArrayItem(b1->cl, pos), taArrayItem(b1->cl, pos - 1), info->equalpct) &&
     taArrayItem(b1->cl, pos - 1) - taArrayItem(b1->op, pos) >=
        taArrayItem(b1->op, pos - 1) - taArrayItem(b1->cl, pos - 1))
  return (taTRUE);
 else
  return (taFALSE);
}

int DLL_EXPORT taDarkCloudCover(taBars *b1, struct taCandleInfo *info, int pos)
{
 if (pos < 1)
  return (taFALSE);
 if (info->candle[pos].color == taBLACKCNDL &&
     info->candle[pos - 1].color == taWHITECNDL &&
     info->candle[pos - 1].shape == taLARGE &&
     taArrayItem(b1->op, pos) > taArrayItem(b1->hi, pos - 1) &&
     taArrayItem(b1->cl, pos) < taBodyMidpoint(b1, pos - 1))
  return (taTRUE);
 else
  return (taFALSE);
}

int DLL_EXPORT taEngulfingLineBear(taBars *b1, struct taCandleInfo *info, int pos)
{
 if (pos < 1)
  return (taFALSE);
 if (info->candle[pos].color == taBLACKCNDL &&
     info->candle[pos - 1].color == taWHITECNDL &&
     info->candle[pos].shape == taLARGE &&
     info->candle[pos - 1].shape == taSMALL &&
     taArrayItem(b1->op, pos) > taArrayItem(b1->cl, pos - 1) &&
     taArrayItem(b1->cl, pos) < taArrayItem(b1->op, pos - 1))
  return (taTRUE);
 else
  return (taFALSE);
}

int DLL_EXPORT taEngulfingLineBull(taBars *b1, struct taCandleInfo *info, int pos)
{
 if (pos < 1)
  return (taFALSE);
 if (info->candle[pos].color == taWHITECNDL &&
     info->candle[pos - 1].color == taBLACKCNDL &&
     info->candle[pos].shape == taLARGE &&
     info->candle[pos - 1].shape == taSMALL &&
     taArrayItem(b1->op, pos) < taArrayItem(b1->cl, pos - 1) &&
     taArrayItem(b1->cl, pos) > taArrayItem(b1->op, pos - 1))
  return (taTRUE);
 else
  return (taFALSE);
}

int DLL_EXPORT taEveningDojiStar(taBars *b1, struct taCandleInfo *info, int pos)
{
 if (pos < 2)
  return (taFALSE);
 if (info->candle[pos].color == taBLACKCNDL &&
     info->candle[pos - 2].color == taWHITECNDL &&
     info->candle[pos - 1].shape == taDOJI &&
     info->candle[pos - 2].shape == taLARGE &&
     taBodyGapUp(b1, pos - 1) &&
     taArrayItem(b1->cl, pos) < taBodyMidpoint(b1, pos - 2))
  return (taTRUE);
 else
  return (taFALSE);
}

int DLL_EXPORT taEveningStar(taBars *b1, struct taCandleInfo *info, int pos)
{
 if (pos < 2)
  return (taFALSE);
 if (info->candle[pos].color == taBLACKCNDL &&
     info->candle[pos - 2].color == taWHITECNDL &&
     info->candle[pos - 1].shape == taSMALL &&
     info->candle[pos - 2].shape == taLARGE &&
     taBodyGapUp(b1, pos - 1) &&
     taArrayItem(b1->cl, pos) < taBodyMidpoint(b1, pos - 2))
  return (taTRUE);
 else
  return (taFALSE);
}


int DLL_EXPORT taFalling3Method(taBars *b1, struct taCandleInfo *info, int pos)
{
 if (pos < 4)
  return (taFALSE);
 if (info->candle[pos].color == taBLACKCNDL &&
     info->candle[pos - 1].color == taWHITECNDL &&
     info->candle[pos - 2].color == taWHITECNDL &&
     info->candle[pos - 3].color == taWHITECNDL &&
     info->candle[pos - 4].color == taBLACKCNDL &&
     info->candle[pos].shape == taLARGE &&
     info->candle[pos - 1].shape == taSMALL &&
     info->candle[pos - 2].shape == taSMALL &&
     info->candle[pos - 3].shape == taSMALL &&
     info->candle[pos - 4].shape == taLARGE &&
     taArrayItem(b1->cl, pos) < taArrayItem(b1->cl, pos - 4) &&
     taArrayItem(b1->cl, pos - 4) < taArrayItem(b1->op, pos - 3) &&
     taArrayItem(b1->cl, pos - 3) < taArrayItem(b1->cl, pos - 2) &&
     taArrayItem(b1->cl, pos - 2) < taArrayItem(b1->cl, pos - 1) &&
     taArrayItem(b1->cl, pos - 1) <= max(taArrayItem(b1->op, pos), taArrayItem(b1->op, pos - 4)))
  return (taTRUE);
 else
  return (taFALSE);
}


int DLL_EXPORT taGravestoneDoji(taBars *b1, struct taCandleInfo *info, int pos)
{
 if (info->candle[pos].shape == taDOJI &&
     taArrayItem(b1->lo, pos) == min(taArrayItem(b1->op, pos), taArrayItem(b1->cl, pos)))
  return (taTRUE);
 else
  return (taFALSE);
}

int DLL_EXPORT taHammer(taBars *b1, struct taCandleInfo *info, int pos)
{
 if (info->candle[pos].color == taBLACKCNDL &&
     info->candle[pos].shape != taLARGE &&
     info->candle[pos].topshadow == taSMALL &&
     info->candle[pos].botshadow == taLARGE)
  return (taTRUE);
 else
  return (taFALSE);
}

int DLL_EXPORT taHangingman(taBars *b1, struct taCandleInfo *info, int pos)
{
 if (info->candle[pos].color == taWHITECNDL &&
     info->candle[pos].shape != taLARGE &&
     info->candle[pos].topshadow == taSMALL &&
     info->candle[pos].botshadow == taLARGE)
  return (taTRUE);
 else
  return (taFALSE);
}

int DLL_EXPORT taHaramiBlack(taBars *b1, struct taCandleInfo *info, int pos)
{                               /* Bottom Signal */
 if (pos < 1)
  return (taFALSE);
 if (info->candle[pos].shape == taSMALL &&
     info->candle[pos - 1].color == taBLACKCNDL &&
     info->candle[pos - 1].shape == taLARGE &&
     taArrayItem(b1->op, pos) > taArrayItem(b1->cl, pos - 1) &&
     taArrayItem(b1->op, pos) < taArrayItem(b1->op, pos - 1) &&
     taArrayItem(b1->cl, pos) > taArrayItem(b1->cl, pos - 1) &&
     taArrayItem(b1->cl, pos) < taArrayItem(b1->op, pos - 1))
  return (taTRUE);
 else
  return (taFALSE);
}

int DLL_EXPORT taHaramiCrossBlack(taBars *b1, struct taCandleInfo *info, int pos)
{                               /* Bottom Signal */
 if (pos < 1)
  return (taFALSE);
 if (info->candle[pos].shape == taDOJI &&
     info->candle[pos - 1].color == taBLACKCNDL &&
     info->candle[pos - 1].shape == taLARGE &&
     taArrayItem(b1->op, pos) > taArrayItem(b1->cl, pos - 1) &&
     taArrayItem(b1->op, pos) < taArrayItem(b1->op, pos - 1) &&
     taArrayItem(b1->cl, pos) > taArrayItem(b1->cl, pos - 1) &&
     taArrayItem(b1->cl, pos) < taArrayItem(b1->op, pos - 1))
  return (taTRUE);
 else
  return (taFALSE);
}

int DLL_EXPORT taHaramiCrossWhite(taBars *b1, struct taCandleInfo *info, int pos)
{                               /* Top Signal */
 if (pos < 1)
  return (taFALSE);
 if (info->candle[pos].shape == taDOJI &&
     info->candle[pos - 1].color == taWHITECNDL &&
     info->candle[pos - 1].shape == taLARGE &&
     taArrayItem(b1->op, pos) < taArrayItem(b1->cl, pos - 1) &&
     taArrayItem(b1->op, pos) > taArrayItem(b1->op, pos - 1) &&
     taArrayItem(b1->cl, pos) < taArrayItem(b1->cl, pos - 1) &&
     taArrayItem(b1->cl, pos) > taArrayItem(b1->op, pos - 1))
  return (taTRUE);
 else
  return (taFALSE);
}


int DLL_EXPORT taHaramiWhite(taBars *b1, struct taCandleInfo *info, int pos)
{                               /* Top Signal */
 if (pos < 1)
  return (taFALSE);
 if (info->candle[pos].shape == taSMALL &&
     info->candle[pos - 1].color == taWHITECNDL &&
     info->candle[pos - 1].shape == taLARGE &&
     taArrayItem(b1->op, pos) < taArrayItem(b1->cl, pos - 1) &&
     taArrayItem(b1->op, pos) > taArrayItem(b1->op, pos - 1) &&
     taArrayItem(b1->cl, pos) < taArrayItem(b1->cl, pos - 1) &&
     taArrayItem(b1->cl, pos) > taArrayItem(b1->op, pos - 1))
  return (taTRUE);
 else
  return (taFALSE);
}

int DLL_EXPORT taHighWave(taBars *b1, struct taCandleInfo *info, int pos)     /* Grp of these
                                                         * indicates reversal */
{
 if (info->candle[pos].shape == taSMALL &&
     (info->candle[pos].topshadow == taLARGE || info->candle[pos].botshadow == taLARGE))
  return (taTRUE);
 else
  return (taFALSE);
}

int DLL_EXPORT taInNeckLine(taBars *b1, struct taCandleInfo *info, int pos)
{
 if (pos < 1)
  return (taFALSE);
 if (info->candle[pos].color == taWHITECNDL &&
     info->candle[pos - 1].color == taBLACKCNDL &&
     info->candle[pos].shape == taSMALL &&
     info->candle[pos - 1].shape != taDOJI &&
     taApproxEqual(taArrayItem(b1->cl, pos), taArrayItem(b1->lo, pos - 1), info->equalpct) &&
     taArrayItem(b1->cl, pos) > taArrayItem(b1->lo, pos - 1))
  return (taTRUE);
 else
  return (taFALSE);
}

int DLL_EXPORT taInvertedHammer(taBars *b1, struct taCandleInfo *info, int pos)       /* Bullish */
{
 if (info->candle[pos].color == taWHITECNDL &&
     info->candle[pos].shape != taLARGE &&
     info->candle[pos].topshadow == taLARGE &&
     info->candle[pos].botshadow == taSMALL)
  return (taTRUE);
 else
  return (taFALSE);
}

int DLL_EXPORT taLongLeggedDoji(taBars *b1, struct taCandleInfo *info, int pos)
{
 if (info->candle[pos].shape == taDOJI &&
     info->candle[pos].topshadow == taLARGE &&
     info->candle[pos].botshadow == taLARGE)
  return (taTRUE);
 else
  return (taFALSE);
}


int DLL_EXPORT taMatHoldPattern(taBars *b1, struct taCandleInfo *info, int pos)
{
 if (pos < 4)
  return (taFALSE);
 if (info->candle[pos].color == taWHITECNDL &&
     info->candle[pos - 1].color == taBLACKCNDL &&
     info->candle[pos - 2].color == taBLACKCNDL &&
     info->candle[pos - 3].color == taBLACKCNDL &&
     info->candle[pos - 4].color == taWHITECNDL &&
     info->candle[pos].shape != taDOJI &&
     info->candle[pos - 1].shape == taSMALL &&
     info->candle[pos - 2].shape == taSMALL &&
     info->candle[pos - 3].shape == taSMALL &&
     info->candle[pos - 4].shape == taLARGE &&
     taBodyGapUp(b1, pos - 3) &&
     taBodyGapUp(b1, pos) &&
     taArrayItem(b1->op, pos - 3) > taArrayItem(b1->op, pos - 2) &&
     taArrayItem(b1->op, pos - 2) > taArrayItem(b1->op, pos - 1))
  return (taTRUE);
 else
  return (taFALSE);
}


int DLL_EXPORT taMorningStar(taBars *b1, struct taCandleInfo *info, int pos)
{
 if (pos < 2)
  return (taFALSE);
 if (info->candle[pos].color == taWHITECNDL &&
     info->candle[pos - 2].color == taBLACKCNDL &&
     info->candle[pos].shape != taDOJI &&
     info->candle[pos - 1].shape == taSMALL &&
     info->candle[pos - 2].shape == taLARGE &&
     taBodyGapDn(b1, pos - 1) &&
     taArrayItem(b1->op, pos) > taArrayItem(b1->op, pos - 1) &&
     taArrayItem(b1->cl, pos) > taBodyMidpoint(b1, pos - 2))
  return (taTRUE);
 else
  return (taFALSE);
}

int DLL_EXPORT taMorningDojiStar(taBars *b1, struct taCandleInfo *info, int pos)
{
 if (pos < 2)
  return (taFALSE);
 if (info->candle[pos].color == taWHITECNDL &&
     info->candle[pos - 2].color == taBLACKCNDL &&
     info->candle[pos].shape != taDOJI &&
     info->candle[pos - 1].shape == taDOJI &&
     info->candle[pos - 2].shape == taLARGE &&
     taBodyGapDn(b1, pos - 1) &&
     taArrayItem(b1->op, pos) > taArrayItem(b1->op, pos - 1) &&
     taArrayItem(b1->cl, pos) > taBodyMidpoint(b1, pos - 2))
  return (taTRUE);
 else
  return (taFALSE);
}

int DLL_EXPORT taPiercingLine(taBars *b1, struct taCandleInfo *info, int pos)
{
 if (pos < 1)
  return (taFALSE);
 if (info->candle[pos].color == taWHITECNDL &&
     info->candle[pos - 1].color == taBLACKCNDL &&
     info->candle[pos].shape != taDOJI &&
     info->candle[pos - 1].shape == taLARGE &&
     taArrayItem(b1->op, pos) < taArrayItem(b1->cl, pos - 1) &&
     taArrayItem(b1->cl, pos) > taBodyMidpoint(b1, pos - 1))
  return (taTRUE);
 else
  return (taFALSE);
}


int DLL_EXPORT taRising3Method(taBars *b1, struct taCandleInfo *info, int pos)
{                               /* bullish pattern */
 if (pos < 4)
  return (taFALSE);
 if (info->candle[pos].color == taWHITECNDL &&
     info->candle[pos - 1].color == taBLACKCNDL &&
     info->candle[pos - 2].color == taBLACKCNDL &&
     info->candle[pos - 3].color == taBLACKCNDL &&
     info->candle[pos - 4].color == taWHITECNDL &&
     info->candle[pos].shape == taLARGE &&
     info->candle[pos - 1].shape == taSMALL &&
     info->candle[pos - 2].shape == taSMALL &&
     info->candle[pos - 3].shape == taSMALL &&
     info->candle[pos - 4].shape == taLARGE &&
     taArrayItem(b1->cl, pos) > taArrayItem(b1->cl, pos - 4) &&
     taArrayItem(b1->cl, pos - 4) > taArrayItem(b1->op, pos - 3) &&
     taArrayItem(b1->cl, pos - 3) > taArrayItem(b1->cl, pos - 2) &&
     taArrayItem(b1->cl, pos - 2) > taArrayItem(b1->cl, pos - 1) &&
     taArrayItem(b1->cl, pos - 1) >= min(taArrayItem(b1->op, pos), taArrayItem(b1->op, pos - 4)))
  return (taTRUE);
 else
  return (taFALSE);
}

int DLL_EXPORT taSeperatingLineBear(taBars *b1, struct taCandleInfo *info, int pos)
{
 if (pos < 1)
  return (taFALSE);
 if (info->candle[pos].color == taBLACKCNDL &&
     info->candle[pos - 1].color == taWHITECNDL &&
     info->candle[pos].shape != taDOJI &&
     info->candle[pos - 1].shape != taDOJI &&
     taArrayItem(b1->op, pos) == taArrayItem(b1->op, pos - 1))
  return (taTRUE);
 else
  return (taFALSE);
}

int DLL_EXPORT taSeperatingLineBull(taBars *b1, struct taCandleInfo *info, int pos)
{
 if (pos < 1)
  return (taFALSE);
 if (info->candle[pos].color == taWHITECNDL &&
     info->candle[pos - 1].color == taBLACKCNDL &&
     info->candle[pos].shape != taDOJI &&
     info->candle[pos - 1].shape != taDOJI &&
     taArrayItem(b1->op, pos) == taArrayItem(b1->op, pos - 1))
  return (taTRUE);
 else
  return (taFALSE);
}

int DLL_EXPORT taShootingStar(taBars *b1, struct taCandleInfo *info, int pos)
{
 if (info->candle[pos].color == taWHITECNDL &&
     info->candle[pos].shape == taSMALL &&
     info->candle[pos].topshadow == taLARGE &&
     info->candle[pos].botshadow == taSMALL)
  return (taTRUE);
 else
  return (taFALSE);
}


int DLL_EXPORT taSideBySideWhiteGapDn(taBars *b1, struct taCandleInfo *info, int pos)
{
 if (pos < 1)
  return (taFALSE);
 if (info->candle[pos].color == taWHITECNDL &&
     info->candle[pos - 1].color == taWHITECNDL &&
     info->candle[pos].shape != taDOJI &&
     info->candle[pos - 1].shape != taDOJI &&
     info->candle[pos].shape == info->candle[pos - 1].shape &&
     taBodyGapDn(b1, pos - 1) &&
     taApproxEqual(taArrayItem(b1->cl, pos), taArrayItem(b1->cl, pos - 1), info->equalpct))
  return (taTRUE);
 else
  return (taFALSE);
}

int DLL_EXPORT taSideBySideWhiteGapUp(taBars *b1, struct taCandleInfo *info, int pos)
{
 if (pos < 1)
  return (taFALSE);
 if (info->candle[pos].color == taWHITECNDL &&
     info->candle[pos - 1].color == taWHITECNDL &&
     info->candle[pos].shape != taDOJI &&
     info->candle[pos - 1].shape != taDOJI &&
     info->candle[pos].shape == info->candle[pos - 1].shape &&
     taBodyGapUp(b1, pos - 1) &&
     taApproxEqual(taArrayItem(b1->op, pos), taArrayItem(b1->op, pos - 1), info->equalpct))
  return (taTRUE);
 else
  return (taFALSE);
}

int DLL_EXPORT taStalledPattern(taBars *b1, struct taCandleInfo *info, int pos)
{
 if (pos < 1)
  return (taFALSE);
 if (info->candle[pos].color == taWHITECNDL &&
     info->candle[pos - 1].color == taWHITECNDL &&
     info->candle[pos].shape == taSMALL &&
     info->candle[pos - 1].shape == taLARGE &&
     taApproxEqual(taArrayItem(b1->op, pos), taArrayItem(b1->cl, pos - 1), info->equalpct))
  return (taTRUE);
 else
  return (taFALSE);
}

int DLL_EXPORT taTasukiDownsideGap(taBars *b1, struct taCandleInfo *info, int pos)
{
 if (pos < 2)
  return (taFALSE);
 if (info->candle[pos].color == taWHITECNDL &&
     info->candle[pos - 1].color == taBLACKCNDL &&
     info->candle[pos - 2].color == taBLACKCNDL &&
     info->candle[pos].shape == info->candle[pos - 1].shape &&
     taArrayItem(b1->op, pos) > taArrayItem(b1->cl, pos - 1) &&
     taArrayItem(b1->cl, pos) > taArrayItem(b1->op, pos - 1) &&
     taArrayItem(b1->cl, pos) < taArrayItem(b1->cl, pos - 2) &&
     taArrayItem(b1->op, pos - 1) < taArrayItem(b1->cl, pos - 2))
  return (taTRUE);
 else
  return (taFALSE);
}

int DLL_EXPORT taTasukiUpsideGap(taBars *b1, struct taCandleInfo *info, int pos)
{
 if (pos < 2)
  return (taFALSE);
 if (info->candle[pos].color == taBLACKCNDL &&
     info->candle[pos - 1].color == taWHITECNDL &&
     info->candle[pos - 2].color == taWHITECNDL &&
     info->candle[pos].shape == info->candle[pos - 1].shape &&
     taArrayItem(b1->op, pos) < taArrayItem(b1->cl, pos - 1) &&
     taArrayItem(b1->cl, pos) < taArrayItem(b1->op, pos - 1) &&
     taArrayItem(b1->cl, pos) > taArrayItem(b1->cl, pos - 2) &&
     taArrayItem(b1->op, pos - 1) > taArrayItem(b1->cl, pos - 2))
  return (taTRUE);
 else
  return (taFALSE);
}

int DLL_EXPORT taThreeGapsDown(taBars *b1, struct taCandleInfo *info, int pos)
{
 if (pos < 2)
  return (taFALSE);
 if (taBodyGapDn(b1, pos) &&
     taBodyGapDn(b1, pos - 1) &&
     taBodyGapDn(b1, pos - 2))
  return (taTRUE);
 else
  return (taFALSE);
}
int DLL_EXPORT taThreeGapsUp(taBars *b1, struct taCandleInfo *info, int pos)
{
 if (pos < 2)
  return (taFALSE);
 if (taBodyGapUp(b1, pos) &&
     taBodyGapUp(b1, pos - 1) &&
     taBodyGapUp(b1, pos - 2))
  return (taTRUE);
 else
  return (taFALSE);
}

int DLL_EXPORT taThreeWhiteSoldiers(taBars *b1, struct taCandleInfo *info, int pos)
{
 if (pos < 2)
  return (taFALSE);
 if (info->candle[pos].color == taWHITECNDL &&
     info->candle[pos - 1].color == taWHITECNDL &&
     info->candle[pos - 2].color == taWHITECNDL &&
     info->candle[pos].topshadow == taSMALL &&
     info->candle[pos - 1].topshadow == taSMALL &&
     info->candle[pos - 2].topshadow == taSMALL &&
     taArrayItem(b1->hi, pos) > taArrayItem(b1->hi, pos - 1) &&
     taArrayItem(b1->hi, pos - 1) > taArrayItem(b1->hi, pos - 2) &&
     taApproxEqual(taArrayItem(b1->hi, pos), taArrayItem(b1->cl, pos), info->equalpct) &&
     taApproxEqual(taArrayItem(b1->hi, pos - 1), taArrayItem(b1->cl, pos - 1), info->equalpct) &&
     taApproxEqual(taArrayItem(b1->hi, pos - 2), taArrayItem(b1->cl, pos - 2), info->equalpct))
  return (taTRUE);
 else
  return (taFALSE);
}

int DLL_EXPORT taThrustingLine(taBars *b1, struct taCandleInfo *info, int pos)
{
 if (pos < 1)
  return (taFALSE);
 if (info->candle[pos].color == taWHITECNDL &&
     info->candle[pos - 1].color == taBLACKCNDL &&
     taArrayItem(b1->cl, pos) > taArrayItem(b1->cl, pos - 1) &&
     taArrayItem(b1->cl, pos) < taBodyMidpoint(b1, pos - 1))
  return (taTRUE);
 else
  return (taFALSE);
}

int DLL_EXPORT taUpsideGap2Crows(taBars *b1, struct taCandleInfo *info, int pos)
{
 if (pos < 2)
  return (taFALSE);
 if (info->candle[pos].color == taBLACKCNDL &&
     info->candle[pos - 1].color == taBLACKCNDL &&
     info->candle[pos - 2].color == taWHITECNDL &&
     info->candle[pos - 2].shape == taLARGE &&
     taBodyGapUp(b1, pos - 1) &&
     taArrayItem(b1->cl, pos) < taArrayItem(b1->cl, pos - 1) &&
     taArrayItem(b1->op, pos) > taArrayItem(b1->op, pos - 1))
  return (taTRUE);
 else
  return (taFALSE);
}


int DLL_EXPORT taCandleColor(struct taCandleInfo *info, int pos)
{
 return (info->candle[pos].color);
}

int DLL_EXPORT taCandleShape(struct taCandleInfo *info, int pos)
{
 return (info->candle[pos].shape);
}

int DLL_EXPORT taCandleTopShadow(struct taCandleInfo *info, int pos)
{
 return (info->candle[pos].topshadow);
}

int DLL_EXPORT taCandleBotShadow(struct taCandleInfo *info, int pos)
{
 return (info->candle[pos].botshadow);
}

