/* Technical Analysis Programmer's Toolkit - Copyright (C) 1997 KTA */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include "ktakit.h"


int DLL_EXPORT taAccumulate(taArray *a1, taArray *a2, int start)
{
    int    pos;

    taArrayItemP(a2, start) = taArrayItemP(a1, start);
    for (pos = start + 1; pos < a1->size && pos < a2->size; pos++)
        taArrayItemP(a2, pos) = taArrayItemP(a2, pos - 1) + taArrayItemP(a1, pos);
    return (0);
}

int DLL_EXPORT taADXR(taBars *b1, taArray *a2, int term1, int term2, int term3, int start)
{
    taArray    adx;
    int    i1, pos;

    if ((i1 = taAllocArray(&adx, b1->size))) return (i1);

    taADX(b1, &adx, term1, term2, start);

    for (pos = start; pos < start + term1 + term2 + term3 - 1 && pos < b1->size && pos < a2->size; pos++)
        taArrayItemP(a2, pos) = 0;
    for (; pos < b1->size && pos < a2->size; pos++)
        taArrayItemP(a2, pos) = (taArrayItem(adx, pos) + taArrayItem(adx, (pos - term3))) / 2;

    taFreeArray(&adx);
    return (0);
}

int DLL_EXPORT taADX(taBars *b1, taArray *a2, int term1, int term2, int start)
{
    taArray    dx;
    int    i1;

    if ((i1 = taAllocArray(&dx, b1->size))) return (i1);

    taDX(b1, &dx, term1, start);
    taWellesMA(&dx, a2, term2, start + term1);

    taFreeArray(&dx);
    return (0);
}

int DLL_EXPORT taArmsEMV(taBars *b1, taArray *a2, long volinc, int start)
{
    int    pos;
    KFloat    pr = 0;               /* price range    */
    KFloat    br = 0;               /* box ratio      */
    KFloat    mpm = 0;              /* midpoint move  */
    KFloat    vol = 0;              /* vol in units of volinc */

    taArrayItemP(a2, start) = 0;
    for (pos = start + 1; pos < b1->size; pos++)
    {
        if (taArrayItem(b1->vol, pos) <= 0 || taArrayItem(b1->hi, pos) == taArrayItem(b1->lo, pos))
            taArrayItemP(a2, pos) = 0;
        else
        {
            pr = (taArrayItem(b1->hi, pos) - taArrayItem(b1->lo, pos)) * 8;
            vol = taArrayItem(b1->vol, pos);
            vol /= volinc;
            br = vol / pr;
            mpm = ((taArrayItem(b1->hi, pos) + taArrayItem(b1->lo, pos)) / 2) -
                  ((taArrayItem(b1->hi, pos - 1) + taArrayItem(b1->lo, pos - 1)) / 2);
            taArrayItemP(a2, pos) = mpm / br;
        }
    }
    return (0);
}

int DLL_EXPORT taArmsIdx(taArray *a1, taArray *a2, taArray *a3, taArray *a4, taArray *a5, int start, int stop)
{
    int    pos;                  /* (adv/dec) / (upvol/dnvol) */
    KFloat    ad;
    KFloat    ud;

    for (pos = start; pos <= stop && pos < a1->size && pos < a5->size; pos++)
    {
        if (taArrayItemP(a1, pos) == 0 || taArrayItemP(a2, pos) == 0) ad = 1;
        else ad = (taArrayItemP(a1, pos) / taArrayItemP(a2, pos));
        if (taArrayItemP(a3, pos) == 0 || taArrayItemP(a4, pos) == 0)
            ud = 1;
        else
            ud = (taArrayItemP(a3, pos) / taArrayItemP(a4, pos));
        taArrayItemP(a5, pos) = ad / ud;
    }
    return (0);
}

int DLL_EXPORT taArrayStats(taArray *a1, KFloat *mini, KFloat *maxi, KFloat *avg, KFloat *sum,
                            int start, int stop, int nonzero)
{
    int    pos, cnt = 0;
    KFloat    point, total = 0;

    if (mini) *mini = taArrayItemP(a1, start);
    if (maxi) *maxi = taArrayItemP(a1, start);
    if (avg) *avg = 0;
    if (sum) *sum = 0;

    for (pos = start; pos <= stop && pos < a1->size; pos++)
    {
        point = taArrayItemP(a1, pos);
        if (mini)
            if (!nonzero || point) *mini = min(*mini, point);
        if (maxi) *maxi = max(*maxi, point);
        if (sum || avg)
            if (!nonzero || point)
            {
                cnt++;
                total += point;
            }
    }
    if (sum) *sum = total;
    if (avg) *avg = total / cnt;
    return (0);
}

int DLL_EXPORT taArrayStatsPos(taArray *a1, int *mini, int *maxi, int start, int stop, int nonzero)
{
    int    pos;

    if (mini) *mini = start;
    if (maxi) *maxi = start;

    for (pos = start; pos <= stop && pos < a1->size; pos++)
    {
        if (mini)
        {
            if (!nonzero || taArrayItemP(a1, pos))
                if (taArrayItemP(a1, pos) < taArrayItemP(a1, *mini)) *mini = pos;
        }
        if (maxi)
        {
            if (taArrayItemP(a1, pos) > taArrayItemP(a1, *maxi)) *maxi = pos;
        }
    }
    return (0);
}

int DLL_EXPORT taATR(taBars *b1, taArray *a2, int term, int start)
{
    int    i1;
    taArray    tr;

    if ((i1 = taAllocArray(&tr, b1->size))) return (i1);
    taTR(b1, &tr, start);
    taWellesMA(&tr, a2, term, start);
    taFreeArray(&tr);
    return (0);
}

KFloat DLL_EXPORT taAvgPrice(taBars *b1, int pos)
{
    int    cnt = 1;
    KFloat    sum;

    sum = taArrayItem(b1->cl, pos);
    if (taArrayItem(b1->op, pos))
    {
        sum += taArrayItem(b1->op, pos);
        cnt++;
    }
    if (taArrayItem(b1->hi, pos))
    {
        sum += taArrayItem(b1->hi, pos);
        cnt++;
    }
    if (taArrayItem(b1->lo, pos))
    {
        sum += taArrayItem(b1->lo, pos);
        cnt++;
    }
    return (sum / cnt);
}

int DLL_EXPORT taBars2PNF(taBars *b1, taBars *b2, taArray *index, KFloat box, KFloat rev, int start)
{
    int    i1, pos1 = 0, pos2 = 0;

    if (!(i1 = taAllocBars(b2, b1->size)))
        if ((i1 = taAllocArray(index, b1->size))) taFreeBars(b2);
    if (i1) return (i1);

    for (pos1 = 0; pos1 <= start && pos1 < b1->size; pos1++)
        taArrayItemP(index, pos1) = pos2;
    taArrayItem(b2->dt, pos2) = taArrayItem(b1->dt, pos1);
    taArrayItem(b2->ti, pos2) = taArrayItem(b1->ti, pos1);
    taArrayItem(b2->hi, pos2) = taArrayItem(b1->cl, pos1);
    taArrayItem(b2->lo, pos2) = taArrayItem(b1->cl, pos1);

    for (pos1++; pos1 < b1->size && fabs(taArrayItem(b1->cl, start) - taArrayItem(b1->cl, pos1)) < box; pos1++)
    {
        if (taArrayItem(b2->lo, pos2) > taArrayItem(b1->cl, pos1))
            taArrayItem(b2->lo, pos2) = taArrayItem(b1->cl, pos1);
        if (taArrayItem(b2->hi, pos2) < taArrayItem(b1->cl, pos1))
            taArrayItem(b2->hi, pos2) = taArrayItem(b1->cl, pos1);
        taArrayItemP(index, pos1) = pos2;
    }
    if (pos1 == b1->size) return (taErrInvalidParm);
    if (taArrayItem(b1->cl, start) > taArrayItem(b1->cl, pos1))
        taArrayItem(b2->oi, pos2) = -1;
    else
        taArrayItem(b2->oi, pos2) = 1;

    for (; pos1 < b1->size; pos1++)
    {
        if (taArrayItem(b2->oi, pos2) == 1)
        {
            if (taArrayItem(b1->cl, pos1) > taArrayItem(b2->hi, pos2))
                taArrayItem(b2->hi, pos2) = taArrayItem(b1->cl, pos1);
            if (taArrayItem(b2->hi, pos2) - taArrayItem(b1->cl, pos1) >= box * rev)
            {
                taArrayItem(b2->cl, pos2) = taArrayItem(b2->hi, pos2);
                taArrayItem(b2->vol, pos2) = floor((taArrayItem(b2->hi, pos2) - taArrayItem(b2->lo, pos2) + box) / box);
                pos2++;
                taArrayItem(b2->oi, pos2) = -1;
                taArrayItem(b2->dt, pos2) = taArrayItem(b1->dt, pos1);
                taArrayItem(b2->ti, pos2) = taArrayItem(b1->ti, pos1);
                taArrayItem(b2->lo, pos2) = taArrayItem(b1->cl, pos1);
                taArrayItem(b2->hi, pos2) = taArrayItem(b2->hi, pos2 - 1) - box;
                taArrayItem(b2->op, pos2) = taArrayItem(b2->hi, pos2);
            }
        }
        else
        {
            if (taArrayItem(b1->cl, pos1) < taArrayItem(b2->lo, pos2))
                taArrayItem(b2->lo, pos2) = taArrayItem(b1->cl, pos1);
            if (taArrayItem(b1->cl, pos1) - taArrayItem(b2->lo, pos2) >= box * rev)
            {
                taArrayItem(b2->cl, pos2) = taArrayItem(b2->lo, pos2);
                taArrayItem(b2->vol, pos2) = floor((taArrayItem(b2->hi, pos2) - taArrayItem(b2->lo, pos2) + box) / box);
                pos2++;
                taArrayItem(b2->oi, pos2) = 1;
                taArrayItem(b2->dt, pos2) = taArrayItem(b1->dt, pos1);
                taArrayItem(b2->ti, pos2) = taArrayItem(b1->ti, pos1);
                taArrayItem(b2->hi, pos2) = taArrayItem(b1->cl, pos1);
                taArrayItem(b2->lo, pos2) = taArrayItem(b2->lo, pos2 - 1) + box;
                taArrayItem(b2->op, pos2) = taArrayItem(b2->lo, pos2);
            }
        }
        taArrayItemP(index, pos1) = pos2;
    }
    if (taArrayItem(b2->oi, pos2) == 1)
        taArrayItem(b2->cl, pos2) = taArrayItem(b2->hi, pos2);
    else
        taArrayItem(b2->cl, pos2) = taArrayItem(b2->lo, pos2);
    taReAllocBars(b2, pos2 + 1);
    return (0);
}

int DLL_EXPORT taBars2PNFhilo(taBars *b1, taBars *b2, taArray *index, KFloat box, KFloat rev, int start)
{
    int    i1, pos1 = 0, pos2 = 0;

    if (!(i1 = taAllocBars(b2, b1->size)))
        if ((i1 = taAllocArray(index, b1->size))) taFreeBars(b2);
    if (i1) return (i1);

    for (pos1 = 0; pos1 <= start && pos1 < b1->size; pos1++)
        taArrayItemP(index, pos1) = pos2;
    taArrayItem(b2->dt, pos2) = taArrayItem(b1->dt, pos1);
    taArrayItem(b2->ti, pos2) = taArrayItem(b1->ti, pos1);
    taArrayItem(b2->hi, pos2) = taArrayItem(b1->hi, pos1);
    taArrayItem(b2->lo, pos2) = taArrayItem(b1->lo, pos1);
    taArrayItem(b2->oi, pos2) = 0;

    for (pos1++; pos1 < b1->size && taArrayItem(b2->oi, pos2) == 0; pos1++)
    {
        if (taArrayItem(b1->hi, pos1) >= taArrayItem(b2->hi, pos2) + box)
        {
            taArrayItem(b2->hi, pos2) = taArrayItem(b1->hi, pos1);
            if (taArrayItem(b1->lo, pos1) > taArrayItem(b2->lo, pos2))
                taArrayItem(b2->oi, pos2) = 1;
        }
        if (taArrayItem(b1->lo, pos1) <= taArrayItem(b2->lo, pos2) - box)
        {
            taArrayItem(b2->lo, pos2) = taArrayItem(b1->lo, pos1);
            if (taArrayItem(b1->hi, pos1) < taArrayItem(b2->hi, pos2))
                taArrayItem(b2->oi, pos2) = -1;
        }
        taArrayItemP(index, pos1) = pos2;
    }
    if (pos1 == b1->size) return (taErrInvalidParm);
    for (; pos1 < b1->size; pos1++)
    {
        if (taArrayItem(b2->oi, pos2) == 1)
        {
            if (taArrayItem(b1->hi, pos1) > taArrayItem(b2->hi, pos2))
                taArrayItem(b2->hi, pos2) = taArrayItem(b1->hi, pos1);
            if (taArrayItem(b1->hi, pos1) <= taArrayItem(b2->hi, pos2) - box * rev)
            {
                taArrayItem(b2->cl, pos2) = taArrayItem(b2->hi, pos2);
                taArrayItem(b2->vol, pos2) = floor((taArrayItem(b2->hi, pos2) - taArrayItem(b2->lo, pos2) + box) / box);
                pos2++;
                taArrayItem(b2->dt, pos2) = taArrayItem(b1->dt, pos1);
                taArrayItem(b2->ti, pos2) = taArrayItem(b1->ti, pos1);
                taArrayItem(b2->vol, pos2) = 0;
                taArrayItem(b2->oi, pos2) = -1;
                taArrayItem(b2->lo, pos2) = taArrayItem(b1->lo, pos1);
                taArrayItem(b2->hi, pos2) = taArrayItem(b2->hi, pos2 - 1) - box;
                taArrayItem(b2->op, pos2) = taArrayItem(b2->hi, pos2);
            }
        }
        else
        {
            if (taArrayItem(b1->lo, pos1) < taArrayItem(b2->lo, pos2))
                taArrayItem(b2->lo, pos2) = taArrayItem(b1->lo, pos1);
            if (taArrayItem(b1->lo, pos1) >= taArrayItem(b2->lo, pos2) + box * rev)
            {
                taArrayItem(b2->cl, pos2) = taArrayItem(b2->lo, pos2);
                taArrayItem(b2->vol, pos2) = floor((taArrayItem(b2->hi, pos2) - taArrayItem(b2->lo, pos2) + box) / box);
                pos2++;
                taArrayItem(b2->dt, pos2) = taArrayItem(b1->dt, pos1);
                taArrayItem(b2->ti, pos2) = taArrayItem(b1->ti, pos1);
                taArrayItem(b2->vol, pos2) = 0;
                taArrayItem(b2->oi, pos2) = 1;
                taArrayItem(b2->hi, pos2) = taArrayItem(b1->hi, pos1);
                taArrayItem(b2->lo, pos2) = taArrayItem(b2->lo, pos2 - 1) + box;
                taArrayItem(b2->op, pos2) = taArrayItem(b2->lo, pos2);
            }
        }
        taArrayItemP(index, pos1) = pos2;
    }
    if (taArrayItem(b2->oi, pos2) == 1)
        taArrayItem(b2->cl, pos2) = taArrayItem(b2->hi, pos2);
    else
        taArrayItem(b2->cl, pos2) = taArrayItem(b2->lo, pos2);
    taReAllocBars(b2, pos2 + 1);
    return (0);
}

int DLL_EXPORT taBarStats(taBars *b1, KFloat *min1, KFloat *max1, int start, int stop)
{
    int    pos = 0;

    if (min1) *min1 = taArrayItem(b1->cl, start);
    if (max1) *max1 = taArrayItem(b1->cl, start);

    for (pos = start; pos <= stop && pos < b1->size; pos++)
    {
        if (min1)
        {
            if (taArrayItem(b1->op, pos))
                *min1 = min(*min1, taArrayItem(b1->op, pos));
            if (taArrayItem(b1->hi, pos))
                *min1 = min(*min1, taArrayItem(b1->hi, pos));
            if (taArrayItem(b1->lo, pos))
                *min1 = min(*min1, taArrayItem(b1->lo, pos));
            if (taArrayItem(b1->cl, pos))
                *min1 = min(*min1, taArrayItem(b1->cl, pos));
        }
        if (max1)
        {
            *max1 = max(*max1, taArrayItem(b1->op, pos));
            *max1 = max(*max1, taArrayItem(b1->hi, pos));
            *max1 = max(*max1, taArrayItem(b1->lo, pos));
            *max1 = max(*max1, taArrayItem(b1->cl, pos));
        }
    }
    return (0);
}

int DLL_EXPORT taBollinger(taBars *b1, taArray *a2, taArray *a3, taArray *a4, KFloat factor,
                           int term, int start)
{
    taArray    tp, std;
    int    pos, i1;

    if (term <= 0) return (taErrInvalidParm);

    if ((i1 = taAllocArray(&tp, b1->size))) return (i1);
    if ((i1 = taAllocArray(&std, b1->size)))
    {
        taFreeArray(&tp);
        return (i1);
    }
    for (pos = start; pos < start + term && pos < b1->size; pos++)
    {
        taArrayItem(std, pos) = 0;
        taArrayItemP(a2, pos) = 0;
        taArrayItemP(a3, pos) = 0;
        taArrayItemP(a4, pos) = 0;
    }

    for (pos = start; pos < b1->size; pos++)
        taArrayItem(tp, pos) = (taArrayItem(b1->hi, pos) + taArrayItem(b1->lo, pos) + taArrayItem(b1->cl, pos)) / 3;
    taSimpleMA(&tp, a3, term, start);
    taStdDevMv(&tp, &std, 0, term, start);
    for (pos = start + term - 1; pos < b1->size; pos++)
    {
        taArrayItemP(a2, pos) = taArrayItemP(a3, pos) + (factor * taArrayItem(std, pos));
        taArrayItemP(a4, pos) = taArrayItemP(a3, pos) - (factor * taArrayItem(std, pos));
    }
    taFreeArray(&std);
    taFreeArray(&tp);
    return (0);
}

int DLL_EXPORT taBoltonTremblay(taArray *a1, taArray *a2, taArray *a3, taArray *a4, int start)
{
    /* ADVANCE   DECLINE   UNCHG     ANSWER  */
    int    pos = 0;
    KFloat    f1 = 0;

    taArrayItemP(a4, start) = 0;
    for (pos = start + 1; pos < a1->size && pos < a2->size && pos < a3->size && pos < a4->size; pos++)
    {
        if (taArrayItemP(a3, pos) == 0)
            taArrayItemP(a4, pos) = taArrayItemP(a4, pos - 1);
        else
        {
            f1 = taArrayItemP(a1, pos) / taArrayItemP(a3, pos) -
                 taArrayItemP(a2, pos) / taArrayItemP(a3, pos);
            if (f1 < 0) taArrayItemP(a4, pos) = taArrayItemP(a4, pos - 1) - sqrt(fabs(f1));
            else taArrayItemP(a4, pos) = taArrayItemP(a4, pos - 1) + sqrt(fabs(f1));
        }
    }
    return (0);
}

int DLL_EXPORT taCCI(taBars *b1, taArray *a2, int term, int start)
{
    int    pos, i1;
    taArray    tp;                   /* temp values */
    taArray    tpavg;                /* temp values */
    taArray    ma;                   /* Moving Averages */
    taArray    md;                   /* Mean deviations */
    KFloat    hi = 0, lo = 0;

    if ((i1 = taAllocArray(&tp, b1->size))) return (i1);
    if ((i1 = taAllocArray(&tpavg, b1->size)))
    {
        taFreeArray(&tp);
        return (i1);
    }
    if ((i1 = taAllocArray(&ma, b1->size)))
    {
        taFreeArray(&tpavg);
        taFreeArray(&tp);
        return (i1);
    }
    if ((i1 = taAllocArray(&md, b1->size)))
    {
        taFreeArray(&ma);
        taFreeArray(&tpavg);
        taFreeArray(&tp);
        return (i1);
    }

    for (pos = start + term - 1; pos < b1->size && pos < a2->size; pos++)
    {
        taArrayStats(&b1->hi, NULL, &hi, NULL, NULL, pos - term + 1, pos, 0);
        taArrayStats(&b1->lo, &lo, NULL, NULL, NULL, pos - term + 1, pos, 0);
        taArrayItem(tp, pos) = (hi + lo + taArrayItem(b1->cl, pos)) / 3;
    }

    taSimpleMA(&tp, &ma, term, start);
    taOscillator(&tp, &ma, &tpavg, start);
    for (pos = start; pos < b1->size; pos++)
        taArrayItem(tpavg, pos) = fabs(taArrayItem(tpavg, pos));
    taSimpleMA(&tpavg, &md, term, start);
    for (pos = start + 3 * term - 3; pos < b1->size && pos < a2->size; pos++)
    {
        taArrayItemP(a2, pos) = (taArrayItem(tp, pos) - taArrayItem(ma, pos))
                                / (taArrayItem(md, pos) * .015);
    }
    taFreeArray(&md);
    taFreeArray(&ma);
    taFreeArray(&tpavg);
    taFreeArray(&tp);
    return (0);
}

int DLL_EXPORT taCFTPP(taBars *b1, taArray *a2, taArray *a3, taArray *a4, taArray *a5, int start)
{
    int    pos;
    KFloat    pp;

    taArrayItemP(a2, start) = 0;
    taArrayItemP(a3, start) = 0;
    taArrayItemP(a4, start) = 0;
    taArrayItemP(a5, start) = 0;
    for (pos = start + 1; pos < b1->size; pos++)
    {
        pp = (taArrayItem(b1->hi, pos) + taArrayItem(b1->lo, pos) + taArrayItem(b1->cl, pos)) / 3;
        taArrayItemP(a2, pos) = pp * 2 - taArrayItem(b1->lo, pos - 1);
        taArrayItemP(a3, pos) = pp + (taArrayItem(b1->hi, pos - 1) - taArrayItem(b1->lo, pos - 1));
        taArrayItemP(a4, pos) = pp * 2 - taArrayItem(b1->hi, pos - 1);
        taArrayItemP(a5, pos) = pp - (taArrayItem(b1->hi, pos - 1) - taArrayItem(b1->lo, pos - 1));
    }
    return (0);
}

int DLL_EXPORT taChaikinMoneyFlow(taBars *b1, taArray *a2, int term, int start)
{
    int    err, pos;   /*  sum((((cl - lo) - (hi - lo)) / (hi - lo)) * vol)  */
    taArray    temp;

    if ((err = taAllocArray(&temp, b1->size))) return (err);
    for (pos = start; pos < b1->size; pos++)
    {
        if (taArrayItem(b1->hi, pos) == taArrayItem(b1->lo, pos))
        {
            if(pos == start || taArrayItem(b1->cl, pos) >= taArrayItem(b1->cl, pos - 1))
                taArrayItem(temp, pos) = 1.0 ;
            else
                taArrayItem(temp, pos) = -1.0 ;
        }
        else
            taArrayItem(temp, pos) =
                (((taArrayItem(b1->cl, pos) - taArrayItem(b1->lo, pos)) -
                  (taArrayItem(b1->hi, pos) - taArrayItem(b1->lo, pos))) /
                 (taArrayItem(b1->hi, pos) - taArrayItem(b1->lo, pos))) *
                taArrayItem(b1->vol, pos);
    }
    taMovingSum(&temp, a2, term, 0);
    taFreeArray(&temp);
    return (0);
}

KFloat DLL_EXPORT taCorCoef(taArray *a1, taArray *a2, int samp, int start, int stop)
{
    int    pos;
    KFloat   accum = 0;
    KFloat   stda1 = 0, stda2 = 0;
    KFloat    avga1 = 0, avga2 = 0;
    KFloat   covarxy = 0, r1 = 0;

    samp = samp ? 1 : 0;
    taArrayStats(a1, NULL, NULL, &avga1, NULL, start, stop, 0);
    taArrayStats(a2, NULL, NULL, &avga2, NULL, start, stop, 0);
    stda1 = taStdDev(a1, samp, start, stop);
    stda2 = taStdDev(a2, samp, start, stop);
    for (pos = start; pos < a1->size && pos < a2->size && pos <= stop; pos++)
    {
        accum += (taArrayItemP(a1, pos) - avga1) * (taArrayItemP(a2, pos) - avga2);
    }
    covarxy = accum / (pos - start - 1);
    r1 = covarxy / (stda1 * stda2);
    return (r1);
}

int DLL_EXPORT taCorCoefMv(taArray *a1, taArray *a2, taArray *a3, int samp, int term, int start)
{
    int    pos;

    for (pos = start; pos < start + term - 1 && pos < a2->size; pos++)
        taArrayItemP(a2, pos) = 0;

    for (pos = start + term - 1; pos < a1->size && pos < a2->size && pos < a3->size; pos++)
        taArrayItemP(a3, pos) = taCorCoef(a1, a2, samp, pos - term + 1, pos);

    return (0);
}

KFloat DLL_EXPORT taCovariance(taArray *a1, taArray *a2, int start, int stop)
{
    int    pos;
    KFloat    a1avg, a2avg, sum = 0;

    stop = min(stop, min(a1->size, a2->size));
    taArrayStats(a1, NULL, NULL, &a1avg, NULL, start, stop, 0);
    taArrayStats(a2, NULL, NULL, &a2avg, NULL, start, stop, 0);

    for (pos = start; pos < a1->size && pos < a2->size && pos <= stop; pos++)
        sum += ((taArrayItemP(a1, pos) - a1avg) * (taArrayItemP(a2, pos) - a2avg));
    return (sum / (stop - start));
}

int DLL_EXPORT taCovarianceMv(taArray *a1, taArray *a2, taArray *a3, int term, int start)
{
    int    pos;

    for (pos = start; pos < start + term - 1 && pos < a2->size && pos < a3->size; pos++)
        taArrayItemP(a3, pos) = 0;
    for (pos = start + term - 1; pos < a1->size && pos < a2->size; pos++)
        taArrayItemP(a3, pos) = taCovariance(a1, a2, pos - term + 1, pos);
    return (0);
}

int DLL_EXPORT taDemandIdx(taBars *b1, taArray *a2, int term, int stock, int start)
{
    int    pos = 0;
    KFloat    c8 = 0, c13 = 0, c20 = 0;
    KFloat   c9 = 0;
    KFloat    pricechg = 0, pctpricechg = 0, volume = 0, bp = 0, sp = 0;
    KFloat    K = 0, bt = 0, st = 0, avgvol = 0, avgprice = 0, normvol = 0;
    KFloat    range = 0, prev_avgprice = 0, prev_avgvol = 0;

    /* recommended initialization values */
    K = 100;
    st = 8;
    bt = 8;
    avgprice = (taArrayItem(b1->hi, start) + taArrayItem(b1->lo, start) +
                2 * taArrayItem(b1->cl, start)) / 4;
    for (pos = start + 1; pos < start + term && pos < b1->size; pos++)
    {
        if (pos == start)
            range += max(taArrayItem(b1->hi, pos), taArrayItem(b1->hi, pos - 1)) -
                     min(taArrayItem(b1->lo, pos), taArrayItem(b1->lo, pos - 1));
        else
            range += taArrayItem(b1->hi, pos) - taArrayItem(b1->lo, pos);
        if (stock) volume += sqrt(taArrayItem(b1->vol, pos));
        else volume += taArrayItem(b1->vol, pos);
    }
    c20 = range / term;
    avgvol = volume / term;
    for (pos = start + term - 1; pos < b1->size; pos++)
    {
        prev_avgprice = avgprice;
        prev_avgvol = avgvol;
        avgprice = (taArrayItem(b1->hi, pos) + taArrayItem(b1->lo, pos) +
                    2 * taArrayItem(b1->cl, pos)) / 4;
        pricechg = avgprice - prev_avgprice;
        if (pricechg < 0) pctpricechg = pricechg / avgprice;
        else pctpricechg = pricechg / prev_avgprice;
        range = max(taArrayItem(b1->hi, pos), taArrayItem(b1->hi, pos - 1)) -
                min(taArrayItem(b1->lo, pos), taArrayItem(b1->lo, pos - 1));
        c20 = (.9 * c20) + (.1 * range);
        K = 3 * avgprice / c20;
        c8 = fabs(pctpricechg * K);
        c9 = exp(c8);
        if (stock) volume = sqrt(taArrayItem(b1->vol, pos));
        else volume = taArrayItem(b1->vol, pos);
        if (volume) normvol = volume / prev_avgvol;
        else normvol = 0;
        c13 = normvol / c9;
        if (pricechg < 0)
        {
            bp = c13;
            sp = normvol;
        }
        else
        {
            bp = normvol;
            sp = c13;
        }
        st = .9 * st + sp;
        bt = .9 * bt + bp;
        taArrayItemP(a2, pos) = bt / st;
        if (taArrayItemP(a2, pos) < 1)
            taArrayItemP(a2, pos) = -taArrayItemP(a2, pos);
        if (taArrayItemP(a2, pos) > 1)
            taArrayItemP(a2, pos) = 1 / taArrayItemP(a2, pos);
        avgvol = (.9 * prev_avgvol) + (.1 * volume);
    }
    return (0);
}

int DLL_EXPORT taDeTrend(taArray *a1, taArray *a2, int start, int stop)
{
    KFloat    slope, constant;
    int    i1 = 0, pos = 0;

    stop = min(stop, min(a1->size, a2->size));
    taRegressionLine(a1, &slope, &constant, start, stop);
    for (i1 = 0, pos = start; pos <= stop; i1++, pos++)
        taArrayItemP(a2, pos) = taArrayItemP(a1, pos) - (slope * i1 + constant);
    return (0);
}

int DLL_EXPORT taDI(taBars *b1, taArray *a2, taArray *a3, int term, int start)
{
    taArray    dmip, dmim, tr, dmips, dmims, trs; /* di = 100 * dmi_sum / tr_sum */
    int    pos = 0, i1;

    if ((i1 = taAllocArray(&dmip, b1->size))) return (i1);
    if ((i1 = taAllocArray(&dmim, b1->size)))
    {
        taFreeArray(&dmip);
        return (i1);
    }
    if ((i1 = taAllocArray(&tr, b1->size)))
    {
        taFreeArray(&dmim);
        taFreeArray(&dmip);
        return (i1);
    }
    if ((i1 = taAllocArray(&dmips, b1->size)))
    {
        taFreeArray(&tr);
        taFreeArray(&dmim);
        taFreeArray(&dmip);
        return (i1);
    }
    if ((i1 = taAllocArray(&dmims, b1->size)))
    {
        taFreeArray(&dmips);
        taFreeArray(&tr);
        taFreeArray(&dmim);
        taFreeArray(&dmip);
        return (i1);
    }
    if ((i1 = taAllocArray(&trs, b1->size)))
    {
        taFreeArray(&dmims);
        taFreeArray(&dmips);
        taFreeArray(&tr);
        taFreeArray(&dmim);
        taFreeArray(&dmip);
        return (i1);
    }
    taDMI(b1, &dmip, &dmim, start);
    taTR(b1, &tr, start);
    taArrayItem(tr, start) = 0;
    taWellesSum(&dmip, &dmips, term, start);
    taWellesSum(&dmim, &dmims, term, start);
    taWellesSum(&tr, &trs, term, start);

    for (pos = start + term; pos < b1->op.size && pos < a2->size && pos < a3->size; pos++)
    {
        taArrayItemP(a2, pos) = 100 * taArrayItem(dmips, pos) / taArrayItem(trs, pos);
        taArrayItemP(a3, pos) = 100 * taArrayItem(dmims, pos) / taArrayItem(trs, pos);
    }

    taFreeArray(&trs);
    taFreeArray(&dmims);
    taFreeArray(&dmips);
    taFreeArray(&tr);
    taFreeArray(&dmim);
    taFreeArray(&dmip);
    return (0);
}

KFloat DLL_EXPORT taDispersion(taArray *a1, int start, int stop)
{
    int    pos;
    KFloat   accum = 0, d1 = 0, result = 0;

    stop = min(a1->size - 1, stop);
    for (pos = start + 1; pos <= stop; pos++)
    {
        d1 = log(taArrayItemP(a1, pos) / taArrayItemP(a1, pos - 1));
        accum += fabs(d1);
    }
    result = sqrt(accum / (stop - start));
    return (result);
}

int DLL_EXPORT taDispersionMv(taArray *a1, taArray *a2, int term, int start)
{
    int    pos;

    for (pos = start; pos < start + term - 1 && pos < a2->size; pos++)
        taArrayItemP(a2, pos) = 0;

    for (pos = start + term - 1; pos < a1->size && pos < a2->size; pos++)
        taArrayItemP(a2, pos) = taDispersion(a1, pos - term + 1, pos);

    return (0);
}

int DLL_EXPORT taDMI(taBars *b1, taArray *a2, taArray *a3, int start)
{
    int    pos = 0;
    KFloat    high = 0, low = 0;

    for (pos = start + 1; pos < b1->op.size && pos < a2->size; pos++)
    {
        high = taArrayItem(b1->hi, pos) - taArrayItem(b1->hi, pos - 1);
        low = taArrayItem(b1->lo, pos - 1) - taArrayItem(b1->lo, pos);
        if ((high < 0) && (low < 0))
        {
            taArrayItemP(a2, pos) = 0;
            taArrayItemP(a3, pos) = 0;
        }
        else
        {
            if (high == low)
            {
                taArrayItemP(a2, pos) = 0;
                taArrayItemP(a3, pos) = 0;
            }
            if (high > low)
            {
                taArrayItemP(a2, pos) = high;
                taArrayItemP(a3, pos) = 0;
            }
            if (high < low)
            {
                taArrayItemP(a2, pos) = 0;
                taArrayItemP(a3, pos) = low;
            }
        }
    }
    return (0);
}

int DLL_EXPORT taDownAverage(taArray *a1, taArray *a2, int term, int start)
{
    int    pos, cnt = 0;
    KFloat    sum = 0;

    taArrayItemP(a2, start) = 0;
    for (pos = start + 1; pos <= start + term && pos < a1->size && pos < a1->size; pos++)
    {
        cnt++;
        if (taArrayItemP(a1, pos) < taArrayItemP(a1, pos - 1))
            sum += taArrayItemP(a1, pos - 1) - taArrayItemP(a1, pos);
        taArrayItemP(a2, pos) = 0;
    }
    taArrayItemP(a2, pos - 1) = sum / cnt;
    for (; pos < a1->size && pos < a1->size; pos++)
    {
        if (taArrayItemP(a1, pos) < taArrayItemP(a1, pos - 1))
            sum = taArrayItemP(a1, pos - 1) - taArrayItemP(a1, pos);
        else
            sum = 0;
        taArrayItemP(a2, pos) = (taArrayItemP(a2, pos - 1) * (term - 1) + sum) / term;
    }
    return (0);
}

int DLL_EXPORT taDX(taBars *b1, taArray *a2, int term, int start)
{
    taArray    dip, dim;             /* (di_plus - di_minus)  */
    int    pos = 0, i1;          /* --------------------  */
    /* (di_plus + di_minus)  */

    if ((i1 = taAllocArray(&dip, b1->size))) return (i1);
    if ((i1 = taAllocArray(&dim, b1->size)))
    {
        taFreeArray(&dip);
        return (i1);
    }
    taDI(b1, &dip, &dim, term, start);

    for (pos = start + term; pos < b1->size && pos < a2->size; pos++)
    {
        taArrayItemP(a2, pos) = 100 *
                                fabs(taArrayItem(dip, pos) - taArrayItem(dim, pos)) /
                                (taArrayItem(dip, pos) + taArrayItem(dim, pos));
    }
    taFreeArray(&dim);
    taFreeArray(&dip);
    return (0);
}

int DLL_EXPORT taEnvelope(taArray *a1, taArray *a2, taArray *a3, taArray *a4, KFloat pct, int start)
{
    int    pos;

    for (pos = start; pos < a1->size && pos < a2->size && pos < a3->size && pos < a4->size; pos++)
    {
        taArrayItemP(a3, pos) = taArrayItemP(a1, pos) + taArrayItemP(a2, pos) * pct;
        taArrayItemP(a4, pos) = taArrayItemP(a1, pos) - taArrayItemP(a2, pos) * pct;
    }
    return (0);
}

int DLL_EXPORT taEnvelopePct(taArray *a1, taArray *a2, taArray *a3, KFloat pct, int start)
{
    int    pos;

    for (pos = start; pos < a1->size && pos < a2->size && pos < a3->size; pos++)
    {
        taArrayItemP(a2, pos) = taArrayItemP(a1, pos) + taArrayItemP(a1, pos) * pct;
        taArrayItemP(a3, pos) = taArrayItemP(a1, pos) - taArrayItemP(a1, pos) * pct;
    }
    return (0);
}

int DLL_EXPORT taExpMA(taArray *a1, taArray *a2, int term, int start)
{
    int    pos;                  /* prev_ma + k * (curr - prev_ma)  */
    KFloat    k1;                   /* k * curr + (1-k) * prev_ma      */
    KFloat    k2;                   /* mathematically same             */

    k1 = 2 / ((float) term + 1);
    k2 = 1 - k1;
    taArrayItemP(a2, start) = taArrayItemP(a1, start);
    for (pos = start + 1; pos < a1->size && pos < a2->size; pos++)
    {
        taArrayItemP(a2, pos) = k1 * taArrayItemP(a1, pos) +
                                k2 * taArrayItemP(a2, pos - 1);
    }
    return (0);
}

KFloat DLL_EXPORT taExpToPeriod(KFloat e1)
{
    return ((2 - e1) / e1);
}

int DLL_EXPORT taFastD(taBars *b1, taArray *a2, int term1, int term2, int start)
{
    int    i1;
    taArray    rawk;

    if ((i1 = taAllocArray(&rawk, b1->size))) return (i1);

    taRawK(b1, &rawk, term1, start);
    taExpMA(&rawk, a2, term2, start + term1 - 1);
    taFreeArray(&rawk);
    return (0);
}

int DLL_EXPORT taFibonacciProject(KFloat f1, KFloat f2, KFloat *project1, KFloat *project2, KFloat *project3)
{
    KFloat   change;

    if (f1 == f2)
    {
        *project1 = *project2 = *project3 = f1;
        return (0);
    }
    change = f2 - f1;
    *project1 = f2 + 1.618 * change;
    *project2 = f2 + 2.618 * change;
    *project3 = f2 + 4.236 * change;
    return (0);
}

int DLL_EXPORT taFibonacciRetrace(KFloat f1, KFloat f2, KFloat *retrace1, KFloat *retrace2, KFloat *retrace3)
{
    KFloat   change;

    if (f1 == f2)
    {
        *retrace1 = *retrace2 = *retrace3 = f1;
        return (0);
    }
    change = f1 - f2;
    *retrace1 = f2 + .382 * change;
    *retrace2 = f2 + .500 * change;
    *retrace3 = f2 + .618 * change;
    return (0);
}

int DLL_EXPORT taFillInterval(taArray *a1, taArray *a2, taArray *a3, int start)
{
    int    i1 = 0, lasttrend = 0, pos = 0, duration = 0;
    KFloat    f1 = 0;

    for (pos = start; pos < a1->size; pos++)
    {

        taArrayItemP(a3, pos) = taArrayItemP(a1, pos);
        if ((taArrayItemP(a2, pos) && pos > 0) || pos == a1->size - 1)
        {
            duration = pos - lasttrend;
            f1 = (taArrayItemP(a1, pos) - taArrayItemP(a1, lasttrend)) / duration;
            for (i1 = lasttrend + 1; i1 < pos; i1++)
                taArrayItemP(a3, i1) = taArrayItemP(a3, i1 - 1) + f1;

            lasttrend = pos;
        }
    }
    return (0);
}

int DLL_EXPORT taGroupBars(taBars *b1, KFloat *op, KFloat *hi, KFloat *lo, KFloat *cl,
        KFloat *vol, KFloat *to, int term, int start)
{
    int    pos = start;

    if (start - term + 1 < 0 || start > b1->op.size) return (taErrInvalidParm);

    *op = taArrayItem(b1->op, pos - term + 1);                            /* First Open      */
    *cl = taArrayItem(b1->cl, pos);                                       /* Last Close      */
    taArrayStats(&b1->hi, NULL, hi, NULL, NULL, pos - term + 1, pos, 0);   /* Highest High    */
    taArrayStats(&b1->lo, lo, NULL, NULL, NULL, pos - term + 1, pos, 0);   /* Lowest Low      */
    taArrayStats(&b1->vol, NULL, NULL, NULL, vol, pos - term + 1, pos, 0); /* Accumulated Vol */
    taArrayStats(&b1->turnov, NULL, NULL, NULL, to, pos - term + 1, pos, 0);   /* Accumulated turnover OI  */
    return (0);
}

int DLL_EXPORT taGroupBarsArray(taBars *b1, taBars *b2, int term, int start)
{
    int    pos;

    if (start + term - 1 < 0 || start > b1->size || start > b2->size)
        return (taErrInvalidParm);

    for (pos = start; pos < start + term && pos < b1->size && pos < b2->size; pos++)
    {
        taArrayItem(b2->op, pos) = taArrayItem(b1->op, pos);
        taArrayItem(b2->hi, pos) = taArrayItem(b1->hi, pos);
        taArrayItem(b2->lo, pos) = taArrayItem(b1->lo, pos);
        taArrayItem(b2->cl, pos) = taArrayItem(b1->cl, pos);
        taArrayItem(b2->vol, pos) = taArrayItem(b1->vol, pos);
        taArrayItem(b2->turnov, pos) = taArrayItem(b1->turnov, pos);
        taArrayItem(b2->oi, pos) = taArrayItem(b1->oi, pos);
        taArrayItem(b2->sprd, pos) = taArrayItem(b1->sprd, pos);
        taArrayItem(b2->dt, pos) = taArrayItem(b1->dt, pos);
        taArrayItem(b2->ti, pos) = taArrayItem(b1->ti, pos);
    }
    for (pos = start + term - 1; pos < b1->size && pos < b2->size; pos++)
    {
        taGroupBars(b1, &taArrayItem(b2->op, pos), &taArrayItem(b2->hi, pos),
                    &taArrayItem(b2->lo, pos), &taArrayItem(b2->cl, pos),
                    &taArrayItem(b2->vol, pos), &taArrayItem(b2->turnov, pos), term, pos);
        taArrayItem(b2->dt, pos) = taArrayItem(b1->dt, pos);
        taArrayItem(b2->ti, pos) = taArrayItem(b1->ti, pos);
    }
    return (0);
}

int DLL_EXPORT taGroupBarsDate(taBars *b1, taBars *b2, int period,
                               unsigned int startdt, unsigned int startti,
                               unsigned int enddt, unsigned int endti)
{
    int    i1, pos = 0, pos2 = 0, newbar = 0;
    int    year0, month0, day0, year1, month1, day1;
    taBars     b3;

    if (period != BY_DAY && period != BY_WEEK && period != BY_MONTH)
        return (taErrInvalidParm);
    if ((i1 = taAllocBars(&b3, b1->size)) != 0) return (i1);

    while (pos < b1->size && (taArrayItem(b1->dt, pos) < startdt ||
                              (taArrayItem(b1->dt, pos) == startdt && taArrayItem(b1->ti, pos) < startti)))
        pos++;

    taArrayItem(b3.op, pos2) = taArrayItem(b1->op, pos);
    taArrayItem(b3.hi, pos2) = taArrayItem(b1->hi, pos);
    taArrayItem(b3.lo, pos2) = taArrayItem(b1->lo, pos);
    taArrayItem(b3.vol, pos2) = taArrayItem(b1->vol, pos);
    taArrayItem(b3.turnov, pos2) = taArrayItem(b1->turnov, pos);
    taArrayItem(b3.oi, pos2) = taArrayItem(b1->oi, pos);
    taArrayItem(b3.sprd, pos2) = taArrayItem(b1->sprd, pos);
    taJulianToYMD(taArrayItem(b1->dt, pos), &year0, &month0, &day0);
    pos++;

    while (pos < b1->size && (taArrayItem(b1->dt, pos) < enddt ||
                              (taArrayItem(b1->dt, pos) == enddt && taArrayItem(b1->ti, pos) <= endti)))
    {
        switch (period)
        {
        case BY_DAY:
            if (taArrayItem(b1->dt, pos) != taArrayItem(b1->dt, pos - 1)) newbar = 1;
            break;
        case BY_WEEK:
            if (taJulianDOW(taArrayItem(b1->dt, pos)) < taJulianDOW(taArrayItem(b1->dt, pos - 1)) ||
                    taArrayItem(b1->dt, pos) - taArrayItem(b1->dt, pos - 1) >= 7)
                newbar = 1;
            break;
        case BY_MONTH:
            taJulianToYMD(taArrayItem(b1->dt, pos), &year1, &month1, &day1);
            if (month1 != month0 || year1 != year0) newbar = 1;
            year0 = year1;
            month0 = month1;
            day0 = day1;
            break;
        }
        if (newbar)
        {
            taArrayItem(b3.dt, pos2) = taArrayItem(b1->dt, pos - 1);
            taArrayItem(b3.ti, pos2) = taArrayItem(b1->ti, pos - 1);
            taArrayItem(b3.cl, pos2) = taArrayItem(b1->cl, pos - 1);
            pos2++;
            newbar = 0;
            if (pos < b1->size)
            {
                taArrayItem(b3.op, pos2) = taArrayItem(b1->op, pos);
                taArrayItem(b3.hi, pos2) = taArrayItem(b1->hi, pos);
                taArrayItem(b3.lo, pos2) = taArrayItem(b1->lo, pos);
                taArrayItem(b3.vol, pos2) = taArrayItem(b1->vol, pos);
                taArrayItem(b3.turnov, pos2) = taArrayItem(b1->turnov, pos);
            }
        }
        else
        {
            taArrayItem(b3.hi, pos2) = max(taArrayItem(b3.hi, pos2), taArrayItem(b1->hi, pos));
            taArrayItem(b3.lo, pos2) = min(taArrayItem(b3.lo, pos2), taArrayItem(b1->lo, pos));
            taArrayItem(b3.vol, pos2) += taArrayItem(b1->vol, pos);
            taArrayItem(b3.turnov, pos2) += taArrayItem(b1->turnov, pos);
        }
        pos++;
    }
    taArrayItem(b3.dt, pos2) = taArrayItem(b1->dt, pos - 1);
    taArrayItem(b3.ti, pos2) = taArrayItem(b1->ti, pos - 1);
    taArrayItem(b3.cl, pos2) = taArrayItem(b1->cl, pos - 1);
    taArrayItem(b3.oi, pos2) = taArrayItem(b1->oi, pos - 1);
    taArrayItem(b3.sprd, pos2) = taArrayItem(b1->sprd, pos - 1);
    pos2++;
    *b2 = b3;
    taReAllocBars(b2, pos2);
    return (0);
}

int DLL_EXPORT taGroupBarsTime(taBars *b1, taBars *b2, unsigned int period,
                               unsigned int startdt, unsigned int startti,
                               unsigned int enddt, unsigned int endti)
{
    int    i1, pos = 0, pos2 = 0, newbar = 0;
    int    year0, month0, day0;
    unsigned  begintime;
    taBars     b3;

    period /= 2;
    if ((i1 = taAllocBars(&b3, b1->size)) != 0) return (i1);

    while (pos < b1->size && (taArrayItem(b1->dt, pos) < startdt ||
                              (taArrayItem(b1->dt, pos) == startdt && taArrayItem(b1->ti, pos) < startti)))
        pos++;

    taArrayItem(b3.op, pos2) = taArrayItem(b1->op, pos);
    taArrayItem(b3.hi, pos2) = taArrayItem(b1->hi, pos);
    taArrayItem(b3.lo, pos2) = taArrayItem(b1->lo, pos);
    taArrayItem(b3.vol, pos2) = taArrayItem(b1->vol, pos);
    taArrayItem(b3.turnov, pos2) = taArrayItem(b1->turnov, pos);
    taArrayItem(b3.oi, pos2) = taArrayItem(b1->oi, pos);
    taArrayItem(b3.sprd, pos2) = taArrayItem(b1->sprd, pos);
    taJulianToYMD(taArrayItem(b1->dt, pos), &year0, &month0, &day0);
    begintime = taArrayItem(b1->ti, pos);
    pos++;

    while (pos < b1->size && (taArrayItem(b1->dt, pos) < enddt ||
                              (taArrayItem(b1->dt, pos) == enddt && taArrayItem(b1->ti, pos) <= endti)))
    {
        if (taArrayItem(b1->dt, pos) != taArrayItem(b1->dt, pos - 1)) newbar = 1;
        if (taArrayItem(b1->ti, pos) - begintime >= period) newbar = 1;
        if (newbar)
        {
            taArrayItem(b3.dt, pos2) = taArrayItem(b1->dt, pos - 1);
            taArrayItem(b3.ti, pos2) = taArrayItem(b1->ti, pos - 1);
            taArrayItem(b3.cl, pos2) = taArrayItem(b1->cl, pos - 1);
            pos2++;
            newbar = 0;
            if (pos < b1->size)
            {
                taArrayItem(b3.op, pos2) = taArrayItem(b1->op, pos);
                taArrayItem(b3.hi, pos2) = taArrayItem(b1->hi, pos);
                taArrayItem(b3.lo, pos2) = taArrayItem(b1->lo, pos);
                taArrayItem(b3.vol, pos2) = taArrayItem(b1->vol, pos);
                taArrayItem(b3.turnov, pos2) = taArrayItem(b1->turnov, pos);
                begintime = taArrayItem(b1->ti, pos);
            }
        }
        else
        {
            taArrayItem(b3.hi, pos2) = max(taArrayItem(b3.hi, pos2), taArrayItem(b1->hi, pos));
            taArrayItem(b3.lo, pos2) = min(taArrayItem(b3.lo, pos2), taArrayItem(b1->lo, pos));
            taArrayItem(b3.vol, pos2) += taArrayItem(b1->vol, pos);
            taArrayItem(b3.turnov, pos2) += taArrayItem(b1->turnov, pos);
        }
        pos++;
    }
    taArrayItem(b3.dt, pos2) = taArrayItem(b1->dt, pos - 1);
    taArrayItem(b3.ti, pos2) = taArrayItem(b1->ti, pos - 1);
    taArrayItem(b3.cl, pos2) = taArrayItem(b1->cl, pos - 1);
    taArrayItem(b3.oi, pos2) = taArrayItem(b1->oi, pos - 1);
    taArrayItem(b3.sprd, pos2) = taArrayItem(b1->sprd, pos - 1);
    pos2++;
    *b2 = b3;
    taReAllocBars(b2, pos2);
    return (0);
}


int DLL_EXPORT taHerrickPayoffIndex(taBars *b1, taArray *a2, KFloat mult, KFloat factor, int start)
{
    int    pos;
    KFloat    diff, mini, hpi;

    taArrayItemP(a2, start) = 0;
    for (pos = start + 1; pos < b1->size; pos++)
    {
        diff = 2 * fabs(taArrayItem(b1->oi, pos) - taArrayItem(b1->oi, pos - 1));
        mini = min(taArrayItem(b1->oi, pos), taArrayItem(b1->oi, pos - 1));
        if (mini == 0)
        {
            hpi = taArrayItemP(a2, pos - 1);
        }
        else
        {
            /*
               hpi = ((taArrayItem(b1->hi, pos) + taArrayItem(b1->lo, pos)) / 2) -
                 ((taArrayItem(b1->hi, pos - 1) + taArrayItem(b1->lo, pos - 1)) / 2) *
                 mult * taArrayItem(b1->vol, pos) * (1 + diff / mini);
            */
            hpi = ((taArrayItem(b1->hi, pos) + taArrayItem(b1->lo, pos)) / 2);
            hpi -= ((taArrayItem(b1->hi, pos - 1) + taArrayItem(b1->lo, pos - 1)) / 2);
            hpi *= mult * taArrayItem(b1->vol, pos) * (1 + diff / mini);
        }
        if (pos == start + 1)
        {
            taArrayItemP(a2, pos) = hpi;
        }
        else
        {
            taArrayItemP(a2, pos) = taArrayItemP(a2, pos - 1) +
                                    ((hpi - taArrayItemP(a2, pos - 1)) * factor);
        }
    }
    return (0);
}

int DLL_EXPORT taLastHigh(taArray *a1, int start, int stop)
{
    int    pos;

    pos = min(a1->size - 1, max(start, stop));
    stop = max(start, stop);

    for (; pos > stop && taArrayItemP(a1, pos) > taArrayItemP(a1, pos - 1); pos--);
    for (; pos > stop && taArrayItemP(a1, pos) < taArrayItemP(a1, pos - 1); pos--);

    if (pos == stop)
    {
        if (taArrayItemP(a1, start) >= taArrayItemP(a1, stop)) return (start);
        else return (stop);
    }
    return (pos);
}

int DLL_EXPORT taLastLow(taArray *a1, int start, int stop)
{
    int    pos;

    pos = min(a1->size - 1, max(start, stop));
    stop = max(start, stop);

    for (pos = start; pos > stop && taArrayItemP(a1, pos) < taArrayItemP(a1, pos - 1); pos--);
    for (; pos > stop && taArrayItemP(a1, pos) > taArrayItemP(a1, pos - 1); pos--);

    if (pos == stop)
    {
        if (taArrayItemP(a1, start) >= taArrayItemP(a1, stop)) return (start);
        else return (stop);
    }
    return (pos);
}

int DLL_EXPORT taLineOsc(taArray *a1, taArray *a2, taArray *a3, int term, int start)
{
    taArray    ma1, ma2;
    int    i1;

    if ((i1 = taAllocArray(&ma1, a1->size))) return (i1);
    if ((i1 = taAllocArray(&ma2, a2->size)))
    {
        taFreeArray(&ma1);
        return (i1);
    }

    taSimpleMA(a1, &ma1, term, start);
    taSimpleMA(a2, &ma2, term, start);
    taOscillator(&ma1, &ma2, a3, start);

    taFreeArray(&ma2);
    taFreeArray(&ma1);
    return (0);
}

int DLL_EXPORT taLstSqrMA(taArray *a1, taArray *a2, int term, int start)
{
    int    pos;
    KFloat    slope, constant;

    for (pos = start; pos < start + term - 1 && pos < a2->size; pos++)
        taArrayItemP(a2, pos) = 0;
    for (pos = start + term - 1; pos < a1->size && pos < a2->size; pos++)
    {
        taRegressionLine(a1, &slope, &constant, pos - term + 1, pos);
        taArrayItemP(a2, pos) = constant + (term + 1) * slope;
    }
    return (0);
}

int DLL_EXPORT taMACD(taArray *a1, taArray *a2, taArray *a3, taArray *a4, taArray *a5,
                      taArray *a6, int term1, int term2, int term3, int start)
{
    int    t1 = 0;

    t1 = min(term1, term2);
    term2 = max(term1, term2);
    term1 = t1;
    taExpMA(a1, a2, term1, start);
    taExpMA(a1, a3, term2, start);
    taOscillator(a2, a3, a4, start);
    taExpMA(a4, a5, term3, start);
    taCrossOverFill(a4, a5, a6, start, a1->size - 1);
    return (0);
}

int DLL_EXPORT taMAEnvelope(taArray *a1, taArray *a2, taArray *a3, KFloat pct, int type, int term, int start)
{
    int    i1;
    taArray    ma;

    if ((i1 = taAllocArray(&ma, a1->size))) return (i1);

    switch (type)
    {
    case ExpMA:
        taExpMA(a1, &ma, term, start);
        break;
    case LstSqrMA:
        taLstSqrMA(a1, &ma, term, start);
        break;
    case SimpleMA:
        taSimpleMA(a1, &ma, term, start);
        break;
    case WeightedMA:
        taWeightedMA(a1, &ma, term, start);
        break;
    case WellesMA:
        taWellesMA(a1, &ma, term, start);
        break;
    default:
        taFreeArray(&ma);
        return (taErrInvalidParm);
    }
    taEnvelopePct(&ma, a2, a3, pct, start + term - 1);
    taFreeArray(&ma);
    return (0);
}

int DLL_EXPORT taMAHighLow(taBars *b1, taArray *a2, taArray *a3, int type, int term, int start)
{
    switch (type)
    {
    case ExpMA:
        taExpMA(&b1->hi, a2, term, start);
        taExpMA(&b1->lo, a3, term, start);
        break;
    case SimpleMA:
        taSimpleMA(&b1->hi, a2, term, start);
        taSimpleMA(&b1->lo, a3, term, start);
        break;
    case LstSqrMA:
        taLstSqrMA(&b1->hi, a2, term, start);
        taLstSqrMA(&b1->lo, a3, term, start);
        break;
    case WeightedMA:
        taWeightedMA(&b1->hi, a2, term, start);
        taWeightedMA(&b1->lo, a3, term, start);
        break;
    case WellesMA:
        taWellesMA(&b1->hi, a2, term, start);
        taWellesMA(&b1->lo, a3, term, start);
        break;
    default:
        return (taErrInvalidParm);
    }
    return (0);
}

int DLL_EXPORT taMcClellanOscSum(taArray *a1, taArray *a2, taArray *a3, taArray *a4,
            int term1, int term2, int start)
{
    taArray    diff1, ma1, ma2;
    int    pos = 0, i1 = 0;

    if ((i1 = taAllocArray(&ma1, a1->size))) return (i1);
    if ((i1 = taAllocArray(&ma2, a1->size)))
    {
        taFreeArray(&ma1);
        return (i1);
    }
    if ((i1 = taAllocArray(&diff1, a1->size)))
    {
        taFreeArray(&ma1);
        taFreeArray(&ma2);
        return (i1);
    }

    taOscillator(a1, a2, &diff1, start);
    taExpMA(&diff1, &ma1, term1, start);
    taExpMA(&diff1, &ma2, term2, start);
    taOscillator(&ma1, &ma2, a3, start);
    for (pos = 0; pos < a1->size; pos++)
    {
        if (pos == 0) taArrayItemP(a4, pos) = taArrayItemP(a3, pos);
        else taArrayItemP(a4, pos) = taArrayItemP(a4, pos - 1) + taArrayItemP(a3, pos);
    }
    taFreeArray(&diff1);
    taFreeArray(&ma2);
    taFreeArray(&ma1);
    return (0);
}

int DLL_EXPORT taMFI(taBars *b1, taArray *a2, int start)
{
    int    pos;
    KFloat    constant;

    taArrayStats(&b1->vol, NULL, NULL, &constant, NULL, start, b1->size, 1);
    for (pos = start; pos < b1->size && pos < a2->size; pos++)
    {
        if (taArrayItem(b1->vol, pos))
            taArrayItemP(a2, pos) = constant *
                            (taArrayItem(b1->hi, pos) - taArrayItem(b1->lo, pos)) / taArrayItem(b1->vol, pos);
        else
            taArrayItemP(a2, pos) = 0;
    }
    return (0);
}

int DLL_EXPORT taMomentum(taArray *a1, taArray *a2, int term, int start)
{
    int    pos;

    for (pos = start; pos < start + term - 1 && pos < a1->size && pos < a2->size; pos++)
        taArrayItemP(a2, pos) = 0;
    for (pos = start + term; pos < a1->size && pos < a2->size; pos++)
    {
        taArrayItemP(a2, pos) = taArrayItemP(a1, pos) - taArrayItemP(a1, (pos - term));
    }
    return (0);
}

int DLL_EXPORT taMovingSum(taArray *a1, taArray *a2, int term, int start)
{
    int    pos;
    KFloat    sum = 0;

    for (pos = start; pos < start + term && pos < a2->size; pos++)
    {
        sum += taArrayItemP(a1, pos);
        taArrayItemP(a2, pos) = 0;
    }
    if (pos == start + term) taArrayItemP(a2, pos - 1) = sum;
    for (; pos < a1->size && pos < a2->size; pos++)
    {
        sum += taArrayItemP(a1, pos);
        sum -= taArrayItemP(a1, pos - term);
        taArrayItemP(a2, pos) = sum;
    }
    return (0);
}

KFloat DLL_EXPORT taOBOi(taBars *b1, int start, int stop)
{
    int    pos;
    KFloat    obi = 0;

    obi = taArrayItem(b1->oi, start);
    for (pos = start + 1; pos < b1->size && pos <= stop; pos++)
    {
        if (taArrayItem(b1->cl, pos) < taArrayItem(b1->cl, pos - 1))
            obi -= taArrayItem(b1->oi, pos);
        else
            obi += taArrayItem(b1->oi, pos);
    }
    return (obi);
}

int DLL_EXPORT taOBOiMv(taBars *b1, taArray *a2, int term, int start)
{
    int    pos;

    for (pos = start; pos < start + term - 1 && pos < a2->size; pos++)
        taArrayItemP(a2, pos) = 0;

    for (pos = start + term - 1; pos < b1->size && pos < a2->size; pos++)
        taArrayItemP(a2, pos) = taOBOi(b1, pos - term + 1, pos);

    return (0);
}

KFloat DLL_EXPORT taOBVol(taBars *b1, int start, int stop)
{
    int    pos;
    KFloat    obv = 0;

    obv = taArrayItem(b1->vol, start);
    for (pos = start + 1; pos < b1->size && pos <= stop; pos++)
    {
        if (taArrayItem(b1->cl, pos) < taArrayItem(b1->cl, pos - 1))
            obv -= taArrayItem(b1->vol, pos);
        else
            obv += taArrayItem(b1->vol, pos);
    }
    return (obv);
}

int DLL_EXPORT taOBVolExp(taBars *b1, taArray *obv, taArray *breakout,
                taArray *fieldtrend, long seedvol, int start)
{
    int    pos;
    KFloat    lotarget = 0, hitarget = 0, currentbreakout = 0, direction = 0;
    KFloat    significantobv[4] = {0, 0, 0, 0};

    taArrayItemP(obv, start) = taArrayItem(b1->vol, start) + seedvol;
    taArrayItemP(breakout, start) = 0;
    taArrayItemP(fieldtrend, start) = 0;

    for (pos = start + 1; pos < b1->size; pos++)
    {
        if (taArrayItem(b1->cl, pos) == taArrayItem(b1->cl, pos - 1))
        {
            taArrayItemP(obv, pos) = taArrayItemP(obv, pos - 1);
            direction = 0;
        }
        if (taArrayItem(b1->cl, pos) < taArrayItem(b1->cl, pos - 1))
        {
            taArrayItemP(obv, pos) = taArrayItemP(obv, pos - 1) - taArrayItem(b1->vol, pos);
            if (direction != -1 && pos > start + 1)
                hitarget = taArrayItemP(obv, pos - 1);
            direction = -1;
        }
        if (taArrayItem(b1->cl, pos) > taArrayItem(b1->cl, pos - 1))
        {
            taArrayItemP(obv, pos) = taArrayItemP(obv, pos - 1) + taArrayItem(b1->vol, pos);
            if (direction != 1 && pos > start + 1) lotarget = taArrayItemP(obv, pos - 1);
            direction = 1;
        }
        taArrayItemP(breakout, pos) = 0;
        if (taArrayItemP(obv, pos) >= hitarget)
        {
            hitarget = taArrayItemP(obv, pos);
            if (pos != start + 1)        /* on second guy we don't want a 1 */
                taArrayItemP(breakout, pos) = 1;
        }
        if (taArrayItemP(obv, pos) <= lotarget)
        {
            lotarget = taArrayItemP(obv, pos);
            if (pos != start + 1)        /* on second guy we don't want a -1 */
                taArrayItemP(breakout, pos) = -1;
        }
        taArrayItemP(fieldtrend, pos) = taArrayItemP(fieldtrend, pos - 1);
        if (taArrayItemP(breakout, pos))
        {
            if (currentbreakout != taArrayItemP(breakout, pos))
            {
                currentbreakout = taArrayItemP(breakout, pos);
                significantobv[0] = significantobv[1];
                significantobv[1] = significantobv[2];
                significantobv[2] = significantobv[3];
            }
            significantobv[3] = taArrayItemP(obv, pos);
            if (significantobv[0])
            {
                if (significantobv[0] > significantobv[2] && significantobv[1] > significantobv[3])
                    taArrayItemP(fieldtrend, pos) = -1;
                else if (significantobv[0] < significantobv[2] && significantobv[1] < significantobv[3])
                    taArrayItemP(fieldtrend, pos) = 1;
                else
                {
                    if (taArrayItemP(fieldtrend, pos - 1) == -1)
                    {
                        if ((taArrayItemP(breakout, pos) == 1 && significantobv[1] < significantobv[3]) ||
                                (taArrayItemP(breakout, pos) == -1 && significantobv[0] < significantobv[2]))
                        {
                            taArrayItemP(fieldtrend, pos) = 0;
                            significantobv[0] = significantobv[1] = 0;
                        }
                    }
                    if (taArrayItemP(fieldtrend, pos - 1) == 1)
                    {
                        if ((taArrayItemP(breakout, pos) == -1 && significantobv[1] > significantobv[3]) ||
                                (taArrayItemP(breakout, pos) == 1 && significantobv[0] > significantobv[2]))
                        {
                            taArrayItemP(fieldtrend, pos) = 0;
                            significantobv[0] = significantobv[1] = 0;
                        }
                    }
                }
            }
        }
    }
    return (0);
}

int DLL_EXPORT taOBVolMv(taBars *b1, taArray *a2, int term, int start)
{
    int    pos;

    for (pos = start; pos < start + term - 1 && pos < a2->size; pos++)
        taArrayItemP(a2, pos) = 0;

    for (pos = start + term - 1; pos < b1->size && pos < a2->size; pos++)
        taArrayItemP(a2, pos) = taOBVol(b1, pos - term + 1, pos);

    return (0);
}

int DLL_EXPORT taOscillator(taArray *a1, taArray *a2, taArray *a3, int start)
{
    int    pos;

    for (pos = start; pos < a1->size && pos < a2->size && pos < a3->size; pos++)
    {
        taArrayItemP(a3, pos) = taArrayItemP(a1, pos) - taArrayItemP(a2, pos);
    }
    return (0);
}

KFloat DLL_EXPORT taPeriodToExp(KFloat n1)
{
    return (2 / (n1 + 1));
}

int DLL_EXPORT taRateOfChange(taArray *a1, taArray *a2, int factor, int term, int start)
{
    int    pos;

    for (pos = start; pos < start + term - 1 && pos < a1->size && pos < a2->size; pos++)
        taArrayItemP(a2, pos) = 0;
    for (pos = start + term; pos < a1->size && pos < a2->size; pos++)
    {
        if (taArrayItemP(a1, pos - term))
            taArrayItemP(a2, pos) = (taArrayItemP(a1, pos) / taArrayItemP(a1, (pos - term))) * factor;
        else
            taArrayItemP(a2, pos) = 0;
    }
    return (0);
}

int DLL_EXPORT taRatio(taArray *a1, taArray *a2, taArray *a3, int start)
{
    int    pos;

    for (pos = start; pos < a1->size && pos < a2->size && pos < a3->size; pos++)
    {
        if (taArrayItemP(a2, pos))
            taArrayItemP(a3, pos) = taArrayItemP(a1, pos) / taArrayItemP(a2, pos);
        else
            taArrayItemP(a3, pos) = 0;
    }
    return (0);
}

int DLL_EXPORT taRawK(taBars *b1, taArray *a2, int term, int start)
{
    int    pos;                  /*       (last - lowest_low)        */
    KFloat    high = 0, low = 0;    /* 100 * -------------------------  */
    /*       (highest_hi - lowest_low)  */

    for (pos = start + term - 1; pos < b1->op.size && pos < a2->size; pos++)
    {
        taArrayStats(&b1->hi, NULL, &high, NULL, NULL, pos - term + 1, pos, 0);
        taArrayStats(&b1->lo, &low, NULL, NULL, NULL, pos - term + 1, pos, 0);
        if (high == low)
            taArrayItemP(a2, pos) = 100;
        else
            taArrayItemP(a2, pos) = 100 * (taArrayItem(b1->cl, pos) - low) / (high - low);
    }
    return (0);
}

int DLL_EXPORT taRegressionLine(taArray *a1, KFloat *slope, KFloat *constant, int start, int stop)
{
    int    pos;
    int    x1 = 1;
    KFloat    sum_of_y = 0, sum_of_y_sqrd = 0, sum_of_xy = 0;
    KFloat    sum_of_x = 0, sum_of_x_sqrd = 0;
    KFloat    f1 = 0, f2 = 0, y1 = 0;

    for (pos = start; pos <= stop && pos < a1->size; pos++)
    {
        y1 = taArrayItemP(a1, pos);
        sum_of_y += y1;
        sum_of_x += x1;
        sum_of_xy += (x1 * y1);
        sum_of_x_sqrd += (x1 * x1);
        sum_of_y_sqrd += (y1 * y1);
        x1++;
    }
    f1 = (((stop - start) + 1) * sum_of_xy) - (sum_of_x * sum_of_y);
    f2 = (((stop - start) + 1) * sum_of_x_sqrd) - (sum_of_x * sum_of_x);
    *slope = f1 / f2;
    *constant = (sum_of_y / (stop - start + 1)) -
                (*slope * (sum_of_x / (stop - start + 1)));
    return (0);
}

int DLL_EXPORT taRegressionLineMv(taArray *a1, taArray *a2, taArray *a3, int term, int start, int stop)
{
    int    pos;
    KFloat    slope, constant;

    for (pos = start + term - 1; pos < a1->size && pos < a2->size && pos <= stop; pos++)
    {
        taRegressionLine(a1, &slope, &constant, pos - term + 1, pos);
        taArrayItemP(a2, pos) = slope;
        taArrayItemP(a3, pos) = constant;
    }
    return (0);
}

int DLL_EXPORT taRSI(taArray *a1, taArray *a2, int term, int start)
{
    int    pos, i1;              /* 100 - 100 / (1 + UpAvg / DnAvg) */
    taArray    upavg, dnavg;

    if ((i1 = taAllocArray(&upavg, a1->size))) return (i1);
    if ((i1 = taAllocArray(&dnavg, a1->size)))
    {
        taFreeArray(&upavg);
        return (i1);
    }
    taUpAverage(a1, &upavg, term, start);
    taDownAverage(a1, &dnavg, term, start);
    for (pos = term; pos < a1->size && pos < a1->size; pos++)
    {
        taArrayItemP(a2, pos) = 100 -
                                (100 / (1 + taArrayItem(upavg, pos) / max(taArrayItem(dnavg, pos), .001)));
    }
    taFreeArray(&dnavg);
    taFreeArray(&upavg);
    return (0);
}

int DLL_EXPORT taRSIC(taBars *b1, taBars *b2, taArray *a1, int term, unsigned int startdate)
{
    int    pos = 0;    /* a1 ep     a1 bp */
    int    i1 = 0;     /* -----  -  ----- */
    int    i2 = 0;     /* a2 ep     a2 bp */
    KFloat    f1 = 0;     /* --------------- */
    KFloat    f2 = 0;     /*      a1 bp      */
    /*      -----      */
    /*      a2 bp      */

    /* read up and sync on date */
    for (i1 = 0; i1 < b1->size; i1++)
        if (taArrayItem(b1->dt, i1) >= startdate) break;
    if (i1 == b1->size)
        return (taErrInvalidParm);        /* couldn't find date */

    for (i2 = 0; i2 < b2->size; i2++)
        if (taArrayItem(b2->dt, i2) == taArrayItem(b1->dt, i1))
            break;
    if (i2 == b2->size)
        return (taErrInvalidParm);        /* couldn't find date */

    for (pos = 0; pos < a1->size; pos++)
        taArrayItemP(a1, pos) = 0;
    for (pos = term - 1; pos + i1 < b1->size && pos + i2 < b2->size; pos++)
    {
        f1 = taArrayItem(b1->cl, pos + i1 - term + 1);
        f2 = taArrayItem(b2->cl, pos + i2 - term + 1);
        f2 = max(f2, .001);
        taArrayItemP(a1, pos + i1) = ((taArrayItem(b1->cl, pos + i1) /
                                       taArrayItem(b2->cl, pos + i2)) - (f1 / f2)) / (f1 / f2);
    }
    return (0);
}

int DLL_EXPORT taRWI(taBars *b1, taArray *a2, taArray *a3, int term, int start)
{
    int    pos = 0, i1 = 0, i2 = 0;
    KFloat    avg = 0, rwilo = 0, rwihi = 0;
    taArray    tr, lblo, lbhi;       /* true range, look back lo, look back hi */

    if ((i1 = taAllocArray(&tr, b1->size))) return (i1);
    if ((i1 = taAllocArray(&lblo, b1->size)))
    {
        taFreeArray(&tr);
        return (i1);
    }
    if ((i1 = taAllocArray(&lbhi, b1->size)))
    {
        taFreeArray(&lblo);
        taFreeArray(&tr);
        return (i1);
    }
    for (pos = 0; pos < start && pos < a2->size && pos < a3->size; pos++)
    {
        taArrayItemP(a2, pos) = 0;
        taArrayItemP(a3, pos) = 0;
    }
    taArrayItem(tr, start) = taArrayItem(b1->hi, start) - taArrayItem(b1->lo, start);

    for (pos = start + 1; pos < start + term && pos < b1->size && pos < a2->size; pos++)
    {
        taArrayItem(tr, pos) = max(taArrayItem(b1->hi, pos) - taArrayItem(b1->lo, pos),
                                   taArrayItem(b1->cl, pos - 1) - taArrayItem(b1->lo, pos));
    }
    for (; pos < b1->size; pos++)
    {
        taArrayItem(tr, pos) = max(taArrayItem(b1->hi, pos) - taArrayItem(b1->lo, pos),
                                   taArrayItem(b1->cl, pos - 1) - taArrayItem(b1->lo, pos));
        for (i2 = 0; i2 < term - 1; i2++)
        {
            taArrayStats(&tr, NULL, NULL, &avg, NULL, pos - i2 - 2, pos - 1, 0);
            if (avg != 0)
            {
                taArrayItem(lblo, i2) = (taArrayItem(b1->hi, pos - i2 - 1) - taArrayItem(b1->lo, pos)) /
                                        (avg * sqrt((i2 + 2)));
                taArrayItem(lbhi, i2) = (taArrayItem(b1->hi, pos) - taArrayItem(b1->lo, pos - i2 - 1)) /
                                        (avg * sqrt((i2 + 2)));
            }
        }
        taArrayStats(&lblo, NULL, &rwilo, NULL, NULL, 0, term - 1, 0);
        taArrayItemP(a3, pos) = rwilo;
        taArrayStats(&lbhi, NULL, &rwihi, NULL, NULL, 0, term - 1, 0);
        taArrayItemP(a2, pos) = rwihi;
    }
    taFreeArray(&lbhi);
    taFreeArray(&lblo);
    taFreeArray(&tr);
    return (0);
}

int DLL_EXPORT taScaleArray(taArray *a1, taArray *a2, KFloat amin, KFloat amax,
                            KFloat mini, KFloat maxi, int nonzero, int start)
{
    int    pos;
    KFloat    aheight, height, scale;

    if (amax == 0 && amin == 0)
        taArrayStats(a1, &amin, &amax, NULL, NULL, start, a1->size, nonzero);
    aheight = amax - amin;
    height = maxi - mini;
    scale = fabs(height) / fabs(aheight);

    for (pos = start; pos < a1->size && pos < a2->size; pos++)
        taArrayItemP(a2, pos) = mini + (taArrayItemP(a1, pos) - amin) * scale;
    return (0);
}

int DLL_EXPORT taSimpleMA(taArray *a1, taArray *a2, int term, int start)
{
    int    pos;
    KFloat    f1 = 0;

    for (pos = start; pos < start + term && pos < a2->size; pos++)
    {
        f1 += taArrayItemP(a1, pos);
        taArrayItemP(a2, pos) = 0;
    }
    if (pos == start + term) taArrayItemP(a2, pos - 1) = f1 / term;
    for (; pos < a1->size && pos < a2->size; pos++)
    {
        f1 += taArrayItemP(a1, pos);
        f1 -= taArrayItemP(a1, pos - term);
        taArrayItemP(a2, pos) = f1 / term;
    }
    return (0);
}

KFloat DLL_EXPORT taSlopeArrayPts(taArray *a1, int x1, int x2)
{
    return ((taArrayItemP(a1, x2) - taArrayItemP(a1, x1)) / (x2 - x1));
}

KFloat DLL_EXPORT taSlopePts(KFloat x1, KFloat y1, KFloat x2, KFloat y2)
{
    return ((y2 - y1) / (x2 - x1));
}

int DLL_EXPORT taSlowD(taBars *b1, taArray *a2, int term1, int term2, int term3, int start)
{
    int    i1;
    taArray    rawk, fastd;

    if ((i1 = taAllocArray(&rawk, b1->size))) return (i1);
    if ((i1 = taAllocArray(&fastd, b1->size)))
    {
        taFreeArray(&rawk);
        return (i1);
    }
    taRawK(b1, &rawk, term1, start);
    taExpMA(&rawk, &fastd, term2, start + term1 - 1);
    taExpMA(&fastd, a2, term3, start + term1 - 1);
    taFreeArray(&fastd);
    taFreeArray(&rawk);
    return (0);
}

KFloat DLL_EXPORT taStdDev(taArray *a1, int samp, int start, int stop)
{
    int    pos;
    KFloat    avg;
    KFloat   accum = 0;

    samp = samp ? 1 : 0;
    stop = min(a1->size - 1, stop);
    taArrayStats(a1, NULL, NULL, &avg, NULL, start, stop, 0);

    for (pos = start; pos <= stop; pos++)
    {
        accum += (taArrayItemP(a1, pos) - avg) * (taArrayItemP(a1, pos) - avg);
    }
    return (sqrt(accum / (stop - start + 1 - samp)));
}

int DLL_EXPORT taStdDevMv(taArray *a1, taArray *a2, int samp, int term, int start)
{
    int    pos;

    for (pos = start; pos < start + term - 1 && pos < a2->size; pos++)
        taArrayItemP(a2, pos) = 0;

    for (pos = start + term - 1; pos < a1->size && pos < a2->size; pos++)
        taArrayItemP(a2, pos) = taStdDev(a1, samp, pos - term + 1, pos);

    return (0);
}

int DLL_EXPORT taStochastic(taArray *a1, taArray *a2, int term, int start)
{
    int    pos;                  /*       (last - lowest)    */
    KFloat    high = 0, low = 0;    /* 100 * ------------------ */
    /*       (highest - lowest) */

    for (pos = start + term - 1; pos < a1->size && pos < a2->size; pos++)
    {
        taArrayStats(a1, NULL, &high, NULL, NULL, pos - term + 1, pos, 0);
        taArrayStats(a1, &low, NULL, NULL, NULL, pos - term + 1, pos, 0);
        if (high == low)
            taArrayItemP(a2, pos) = 100;
        else
            taArrayItemP(a2, pos) = 100 * (taArrayItemP(a1, pos) - low) / (high - low);
    }
    return (0);
}

KFloat DLL_EXPORT taSumPrev(taArray *a1, int term, int start)
{
    int    pos = 0, begining = 0;
    KFloat    sum = 0;

    begining = start - term + 1;
    if (begining < 0) begining = 0;
    for (pos = start; pos >= begining; pos--)
        sum += taArrayItemP(a1, pos);
    return (sum);
}

int DLL_EXPORT taTR(taBars *b1, taArray *a2, int start)
{
    int    pos;                  /*            abs(cur_hi - cur_lo)  */
    KFloat   hilo, hicl, locl;     /* largest of abs(cur_hi - prev_cl) */
    /*            abs(cur_lo - prev_cl) */

    taArrayItemP(a2, start) = fabs(taArrayItem(b1->hi, start) -
                                   taArrayItem(b1->lo, start));
    for (pos = start + 1; pos < b1->op.size && pos < a2->size; pos++)
    {
        hilo = fabs(taArrayItem(b1->hi, pos) - taArrayItem(b1->lo, pos));
        hicl = fabs(taArrayItem(b1->hi, pos) - taArrayItem(b1->cl, pos - 1));
        locl = fabs(taArrayItem(b1->lo, pos) - taArrayItem(b1->cl, pos - 1));
        taArrayItemP(a2, pos) = max(max(hilo, hicl), locl);;
    }
    return (0);
}

int DLL_EXPORT taTrendScore(taArray *a1, taArray *a2, int term, int start)
{
    int    pos;
    KFloat    score = 0;

    taArrayItemP(a2, start) = 0;
    for (pos = start + 1; pos <= start + term && pos < a2->size; pos++)
    {
        taArrayItemP(a2, pos) = 0;
        if (taArrayItemP(a1, pos) >= taArrayItemP(a1, pos - 1)) score += 1;
        else score -= 1;
    }
    taArrayItemP(a2, pos - 1) = score;
    for (; pos < a1->size && pos < a2->size; pos++)
    {
        if (taArrayItemP(a1, pos) >= taArrayItemP(a1, pos - 1)) score += 1;
        else score -= 1;
        if (taArrayItemP(a1, pos - term) >= taArrayItemP(a1, pos - term - 1))
            score -= 1;
        else
            score += 1;
        taArrayItemP(a2, pos) = score;
    }
    return (0);
}

KFloat DLL_EXPORT taTrueHigh(taBars *b1, int pos)
{
    if (pos == 0 || taArrayItem(b1->hi, pos) > taArrayItem(b1->cl, pos - 1))
        return (taArrayItem(b1->hi, pos));
    else
        return (taArrayItem(b1->cl, pos - 1));
}

KFloat DLL_EXPORT taTrueLow(taBars *b1, int pos)
{
    if (pos == 0 || taArrayItem(b1->lo, pos) < taArrayItem(b1->cl, pos - 1))
        return (taArrayItem(b1->lo, pos));
    else
        return (taArrayItem(b1->cl, pos - 1));
}

KFloat DLL_EXPORT taTrueRange(taBars *b1, int pos)
{
    return (taTrueHigh(b1, pos) - taTrueLow(b1, pos));
}

int DLL_EXPORT taTSI(taArray *a1, taArray *a2, int term1, int term2, int start)
{
    taArray    mtm, absmtm, longmtm, shortmtm, longamtm, shortamtm;
    int    pos = 0, i1 = 0;

    if ((i1 = taAllocArray(&mtm, a1->size))) return (i1);
    if ((i1 = taAllocArray(&absmtm, a1->size)))
    {
        taFreeArray(&mtm);
        return (i1);
    }
    if ((i1 = taAllocArray(&longmtm, a1->size)))
    {
        taFreeArray(&mtm);
        taFreeArray(&absmtm);
        return (i1);
    }
    if ((i1 = taAllocArray(&shortmtm, a1->size)))
    {
        taFreeArray(&mtm);
        taFreeArray(&absmtm);
        taFreeArray(&longmtm);
        return (i1);
    }
    if ((i1 = taAllocArray(&longamtm, a1->size)))
    {
        taFreeArray(&mtm);
        taFreeArray(&absmtm);
        taFreeArray(&longmtm);
        taFreeArray(&shortmtm);
        return (i1);
    }
    if ((i1 = taAllocArray(&shortamtm, a1->size)))
    {
        taFreeArray(&mtm);
        taFreeArray(&absmtm);
        taFreeArray(&longmtm);
        taFreeArray(&shortmtm);
        taFreeArray(&longamtm);
        return (i1);
    }

    for (pos = start + 1; pos < a1->size; pos++)
    {
        taArrayItem(mtm, pos) = taArrayItemP(a1, pos) - taArrayItemP(a1, pos - 1);
        if (taArrayItem(mtm, pos) < 0)
            taArrayItem(absmtm, pos) = -1 * taArrayItem(mtm, pos);
        else
            taArrayItem(absmtm, pos) = taArrayItem(mtm, pos);
    }

    taExpMA(&mtm, &longmtm, term1, start + 1);
    taExpMA(&longmtm, &shortmtm, term2, start + 1);
    taExpMA(&absmtm, &longamtm, term1, start + 1);
    taExpMA(&longamtm, &shortamtm, term2, start + 1);
    taRatio(&shortmtm, &shortamtm, a2, start + 1);

    taFreeArray(&mtm);
    taFreeArray(&absmtm);
    taFreeArray(&longmtm);
    taFreeArray(&shortmtm);
    taFreeArray(&longamtm);
    taFreeArray(&shortamtm);
    return (0);
}

int DLL_EXPORT taUpAverage(taArray *a1, taArray *a2, int term, int start)
{
    int    pos, cnt = 0;
    KFloat    sum = 0;

    taArrayItemP(a2, start) = 0;
    for (pos = start + 1; pos <= start + term && pos < a1->size && pos < a1->size; pos++)
    {
        cnt++;
        if (taArrayItemP(a1, pos) > taArrayItemP(a1, pos - 1))
            sum += taArrayItemP(a1, pos) - taArrayItemP(a1, pos - 1);
        taArrayItemP(a2, pos) = 0;
    }
    taArrayItemP(a2, pos - 1) = sum / cnt;
    for (; pos < a1->size && pos < a1->size; pos++)
    {
        if (taArrayItemP(a1, pos) > taArrayItemP(a1, pos - 1))
            sum = taArrayItemP(a1, pos) - taArrayItemP(a1, pos - 1);
        else
            sum = 0;
        taArrayItemP(a2, pos) = (taArrayItemP(a2, pos - 1) * (term - 1) + sum) / term;
    }
    return (0);
}

int DLL_EXPORT taUltimateOsc(taBars *b1, taArray *a2, int term1, int term2, int term3, int start)
{
    int    err, pos, maxi;
    taArray    x0, y0, x1, y1;

    if ((err = taAllocArray(&x0, b1->size))) return (err);
    if ((err = taAllocArray(&x1, b1->size)))
    {
        taFreeArray(&x0);
        return (err);
    }
    if ((err = taAllocArray(&y0, b1->size)))
    {
        taFreeArray(&x1);
        taFreeArray(&x0);
        return (err);
    }
    if ((err = taAllocArray(&y1, b1->size)))
    {
        taFreeArray(&y0);
        taFreeArray(&x1);
        taFreeArray(&x0);
        return (err);
    }

    for (pos = start; pos < b1->size; pos++)
    {
        taArrayItem(x0, pos) = taArrayItem(b1->cl, pos) - taTrueLow(b1, pos);
        taArrayItem(y0, pos) = taTrueRange(b1, pos);
    }

    taSimpleMA(&x0, &x1, term1, start);
    taSimpleMA(&y0, &y1, term1, start);
    for (pos = start; pos < b1->size; pos++)
        if (taArrayItem(y1, pos))
            taArrayItemP(a2, pos) = ((taArrayItem(x1, pos) * term1) / (taArrayItem(y1, pos) * term1)) * 4;
        else
            taArrayItemP(a2, pos) = 0;

    taSimpleMA(&x0, &x1, term2, start);
    taSimpleMA(&y0, &y1, term2, start);
    for (pos = start; pos < b1->size; pos++)
        if (taArrayItemP(a2, pos) && taArrayItem(y1, pos))
            taArrayItemP(a2, pos) += ((taArrayItem(x1, pos) * term2) / (taArrayItem(y1, pos) * term2)) * 2;
        else
            taArrayItemP(a2, pos) = 0;

    taSimpleMA(&x0, &x1, term3, start);
    taSimpleMA(&y0, &y1, term3, start);
    for (pos = start; pos < b1->size; pos++)
        if (taArrayItemP(a2, pos) && taArrayItem(y1, pos))
            taArrayItemP(a2, pos) += (taArrayItem(x1, pos) * term3) / (taArrayItem(y1, pos) * term3);
        else
            taArrayItemP(a2, pos) = 0;

    for (pos = start; pos < b1->size; pos++)
        if (taArrayItemP(a2, pos))
            taArrayItemP(a2, pos) /= 7;

    maxi = max(max(term1, term2), term3);
    for (pos = start; pos < maxi; pos++)
        taArrayItemP(a2, pos) = 0;

    taFreeArray(&y1);
    taFreeArray(&y0);
    taFreeArray(&x1);
    taFreeArray(&x0);
    return (0);
}

int DLL_EXPORT taVHF(taArray *a1, taArray *a2, int term, int start)
{
    int    pos;                  /* Highest close - Lowest close */
    KFloat    sum = 0;              /* ---------------------------- */
    KFloat    hicl;                 /* term   close(i) - close(i-1) */
    KFloat    locl;                 /* SUM ABS--------------------- */
    /* i=1          close(i-1)      */

    taArrayItemP(a2, start) = 0;
    for (pos = start + 1; pos <= start + term && pos < a2->size; pos++)
    {
        sum += fabs((taArrayItemP(a1, pos) - taArrayItemP(a1, pos - 1))
                / taArrayItemP(a1, pos - 1));
        taArrayItemP(a2, pos) = 0;
    }
    taArrayStats(a1, &locl, &hicl, NULL, NULL, pos - term, pos - 1, 0);
    taArrayItemP(a2, pos - 1) = (hicl - locl) / sum;
    for (; pos < a1->size && pos < a2->size; pos++)
    {
        taArrayStats(a1, &locl, &hicl, NULL, NULL, pos - term + 1, pos, 0);
        sum += fabs((taArrayItemP(a1, pos) - taArrayItemP(a1, pos - 1))
                / taArrayItemP(a1, pos - 1));
        sum -= fabs((taArrayItemP(a1, pos - term) - taArrayItemP(a1, pos - term - 1))
                / taArrayItemP(a1, pos - term - 1));
        taArrayItemP(a2, pos) = (hicl - locl) / sum;
    }
    return (0);
}

int DLL_EXPORT taWeightedMA(taArray *a1, taArray *a2, int term, int start)
{
    int    pos = 0, fact = 0, i1 = 0;

    fact = term * (term + 1) / 2;
    for (pos = start; pos < start + term && pos < a1->size && pos < a2->size; pos++)
        taArrayItemP(a2, pos) = 0;
    for (pos = start + term - 1; pos < a1->size && pos < a2->size; pos++)
    {
        taArrayItemP(a2, pos) = 0;
        for (i1 = 1; i1 <= term; i1++)
            taArrayItemP(a2, pos) += taArrayItemP(a1, pos - term + i1) * i1;
        taArrayItemP(a2, pos) /= fact;
    }
    return (0);
}

int DLL_EXPORT taWellesMA(taArray *a1, taArray *a2, int term, int start)
{
    int    pos;                  /* (prev_wma * (term - 1) + curr) / term  */
    KFloat    f1 = 0;

    for (pos = start; pos < start + term && pos < a1->size && pos < a2->size; pos++)
    {
        f1 += taArrayItemP(a1, pos);
        taArrayItemP(a2, pos) = 0;
    }
    taArrayItemP(a2, pos - 1) = f1 / term;
    for (; pos < a1->size && pos < a2->size; pos++)
    {
        taArrayItemP(a2, pos) =
            (taArrayItemP(a2, pos - 1) * (term - 1) + taArrayItemP(a1, pos)) / term;
    }
    return (0);
}

int DLL_EXPORT taWellesSum(taArray *a1, taArray *a2, int term, int start)
{
    int    pos;                  /* (prev_sum - (prev_sum / term) + curr  */
    KFloat    sum = 0;

    for (pos = start; pos < start + term && pos < a1->size && pos < a2->size; pos++)
    {
        sum += taArrayItemP(a1, pos);
        taArrayItemP(a2, pos) = 0;
    }
    taArrayItemP(a2, pos - 1) = sum;
    for (; pos < a1->size && pos < a2->size; pos++)
    {
        sum = sum - (sum / term) + taArrayItemP(a1, pos);
        taArrayItemP(a2, pos) = sum;
    }
    return (0);
}

int DLL_EXPORT taWellesVolatility(taBars *b1, taArray *a2, taArray *a3, taArray *a4,
                                taArray *a5, KFloat constant, int term, int start)
{
    taArray    atr;
    int    pos, i1, shortest, position = -1;

    shortest = min(min(min(a2->size, a3->size), a4->size), a5->size);
    if ((i1 = taAllocArray(&atr, b1->size))) return (i1);

    taATR(b1, &atr, term, start);

    for (pos = start + 1; pos < b1->size && pos < shortest; pos++)
    {
        if (pos > start)
            taArrayItemP(a2, pos) = taArrayItemP(a2, pos - 1);
        else
            taArrayItemP(a2, pos) = 0;
        taArrayItemP(a3, pos) = taArrayItem(atr, pos) * constant;
        taArrayItemP(a5, pos) = 0;
        switch (position)
        {
        case 1:   /* LONG */
            taArrayItemP(a4, pos) = taArrayItemP(a2, pos) - taArrayItemP(a3, pos - 1);
            if (taArrayItem(b1->cl, pos) > taArrayItemP(a2, pos))
                taArrayItemP(a2, pos) = taArrayItem(b1->cl, pos);
            if (taArrayItem(b1->cl, pos) < taArrayItemP(a4, pos))
            {
                taArrayItemP(a2, pos) = taArrayItem(b1->cl, pos);
                taArrayItemP(a5, pos) = -1;
                position = -1;
            }
            break;
        case -1:  /* int */
            taArrayItemP(a4, pos) = taArrayItemP(a2, pos) + taArrayItemP(a3, pos - 1);
            if (taArrayItem(b1->cl, pos) < taArrayItemP(a2, pos))
                taArrayItemP(a2, pos) = taArrayItem(b1->cl, pos);
            if (taArrayItem(b1->cl, pos) > taArrayItemP(a4, pos))
            {
                taArrayItemP(a2, pos) = taArrayItem(b1->cl, pos);
                if (pos >= term)
                {
                    taArrayItemP(a5, pos) = 1;
                    position = 1;
                }
            }
            break;
        }
    }
    taFreeArray(&atr);
    return (0);
}

int DLL_EXPORT taWilliamsAD(taBars *b1, taArray *a2, int start)
{
    int    pos;

    taArrayItemP(a2, start) = 0;

    for (pos = start + 1; pos < b1->size && pos < a2->size; pos++)
    {
        if (taArrayItem(b1->cl, pos) > taArrayItem(b1->cl, pos - 1))
            taArrayItemP(a2, pos) = taArrayItemP(a2, pos - 1) +
                                    (taArrayItem(b1->cl, pos) -
                                     min(taArrayItem(b1->lo, pos), taArrayItem(b1->cl, pos - 1)));

        if (taArrayItem(b1->cl, pos) == taArrayItem(b1->cl, pos - 1))
            taArrayItemP(a2, pos) = taArrayItemP(a2, pos - 1);

        if (taArrayItem(b1->cl, pos) < taArrayItem(b1->cl, pos - 1))
            taArrayItemP(a2, pos) = taArrayItemP(a2, pos - 1) -
                                    (max(taArrayItem(b1->hi, pos), taArrayItem(b1->cl, pos - 1)) -
                                     taArrayItem(b1->cl, pos));
    }
    return (0);
}

int DLL_EXPORT taWilliamsR(taBars *b1, taArray *a2, int term, int start)
{
    int    pos;
    KFloat    hi = 0, lo = 0;

    for (pos = start; pos < b1->size && pos < start + term; pos++)
        taArrayItemP(a2, pos) = 0;
    for (pos = start + term - 1; pos < b1->size; pos++)
    {
        taArrayStats(&b1->hi, NULL, &hi, NULL, NULL, pos - term + 1, pos, 0);
        taArrayStats(&b1->lo, &lo, NULL, NULL, NULL, pos - term + 1, pos, 0);
        taArrayItemP(a2, pos) = 100 * (hi - taArrayItem(b1->cl, pos)) / (hi - lo);
    }
    return (0);
}


/* ----- Formation Functions ---------------------------------------------- */

int DLL_EXPORT taFormationFill(taBars *b1, taArray *a2, char *formfunc, int start, int stop)
{
    int    i1, pos, cnt = 0;
    char    *formations[] =
    {
        "taCPR",
        "taGap",
        "taIslandReversal",
        "taInsideBar",
        "taKeyReversal",
        "taOutsideBar",
        ""
    };

    stop = min(stop, min(b1->size - 1, a2->size - 1));
    for (i1 = 0; formations[i1][0]; i1++)
        if (!stricmp(formfunc, formations[i1])) break;
    switch (i1)
    {
    case 0:
        for(pos = start; pos <= stop; pos++)
            cnt += (taArrayItemP(a2, pos) = taCPR(b1, pos)) != 0;
        break;
    case 1:
        for(pos = start; pos <= stop; pos++)
            cnt += (taArrayItemP(a2, pos) = taGap(b1, pos)) != 0;
        break;
    case 2:
        for(pos = start; pos <= stop; pos++)
            cnt += (taArrayItemP(a2, pos) = taIslandReversal(b1, pos)) != 0;
        break;
    case 3:
        for(pos = start; pos <= stop; pos++)
            cnt += (taArrayItemP(a2, pos) = taInsideBar(b1, pos)) != 0;
        break;
    case 4:
        for(pos = start; pos <= stop; pos++)
            cnt += (taArrayItemP(a2, pos) = taKeyReversal(b1, pos)) != 0;
        break;
    case 5:
        for(pos = start; pos <= stop; pos++)
            cnt += (taArrayItemP(a2, pos) = taOutsideBar(b1, pos)) != 0;
        break;
    default:
        return (taErrInvalidParm);
    }
    return (cnt);
}

int DLL_EXPORT taCPR(taBars *b1, int pos)
{
    if (pos == 0) return (taZERO);
    if (taArrayItem(b1->hi, pos) > taArrayItem(b1->hi, pos - 1) &&
            taArrayItem(b1->lo, pos) < taArrayItem(b1->lo, pos - 1))
    {
        if (taArrayItem(b1->cl, pos) > taArrayItem(b1->hi, pos - 1))
            return (taUP);
        if (taArrayItem(b1->cl, pos) < taArrayItem(b1->lo, pos - 1))
            return (taDOWN);
    }
    return (taZERO);
}

int DLL_EXPORT taGap(taBars *b1, int pos)
{
    if (pos == 0) return (taZERO);
    if (taArrayItem(b1->lo, pos) > taArrayItem(b1->hi, pos - 1)) return (taUP);
    if (taArrayItem(b1->hi, pos) < taArrayItem(b1->lo, pos - 1)) return (taDOWN);
    return (taZERO);
}

int DLL_EXPORT taIslandReversal(taBars *b1, int pos)
{
    if (pos == 0) return (taZERO);
    if (taArrayItem(b1->hi, pos) < taArrayItem(b1->lo, pos - 1) &&
            taArrayItem(b1->cl, pos) > taArrayItem(b1->lo, pos) +
            (taArrayItem(b1->hi, pos) - taArrayItem(b1->lo, pos)) * .70)
        return (taUP);
    if (taArrayItem(b1->lo, pos) > taArrayItem(b1->hi, pos - 1) &&
            taArrayItem(b1->cl, pos) < taArrayItem(b1->hi, pos) -
            (taArrayItem(b1->hi, pos) - taArrayItem(b1->lo, pos)) * .70)
        return (taDOWN);
    return (taZERO);
}

int DLL_EXPORT taInsideBar(taBars *b1, int pos)
{
    if (pos == 0) return (taFALSE);
    if (taArrayItem(b1->hi, pos) < taArrayItem(b1->hi, pos - 1) &&
            taArrayItem(b1->lo, pos) > taArrayItem(b1->lo, pos - 1))
        return (taTRUE);
    return (taFALSE);
}

int DLL_EXPORT taKeyReversal(taBars *b1, int pos)
{
    if (pos == 0) return (taZERO);
    if (taArrayItem(b1->lo, pos) < taArrayItem(b1->lo, pos - 1) &&
            taArrayItem(b1->cl, pos) > taArrayItem(b1->cl, pos - 1))
        return (taUP);
    if (taArrayItem(b1->hi, pos) > taArrayItem(b1->hi, pos - 1) &&
            taArrayItem(b1->cl, pos) < taArrayItem(b1->cl, pos - 1))
        return (taDOWN);
    return (taZERO);
}

int DLL_EXPORT taOutsideBar(taBars *b1, int pos)
{
    if (pos == 0) return (taFALSE);
    if (taArrayItem(b1->hi, pos) > taArrayItem(b1->hi, pos - 1) &&
            taArrayItem(b1->lo, pos) < taArrayItem(b1->lo, pos - 1))
        return (taTRUE);
    return (taFALSE);
}

int DLL_EXPORT taCrossOver(taArray *a1, taArray *a2, int pos)
{
    if (pos == 0) return (taZERO);
    if ((taArrayItemP(a1, pos) > taArrayItemP(a2, pos)) &&
            (taArrayItemP(a1, pos - 1) < taArrayItemP(a2, pos - 1)))
        return (taUP);
    else if ((taArrayItemP(a1, pos) < taArrayItemP(a2, pos)) &&
             (taArrayItemP(a1, pos - 1) > taArrayItemP(a2, pos - 1)))
        return (taDOWN);
    else
        return (taZERO);
}

int DLL_EXPORT taCrossOverFill(taArray *a1, taArray *a2, taArray *a3, int start, int stop)
{
    int    pos;

    taArrayItemP(a3, start) = taZERO;
    for (pos = start + 1; pos < stop && pos < a1->size && pos < a2->size && pos < a3->size; pos++)
        taArrayItemP(a3, pos) = taCrossOver(a1, a2, pos);
    return (0);
}

int DLL_EXPORT taTurn(taArray *a1, int pos)
{
    if (pos < 2) return (taZERO);
    if ((taArrayItemP(a1, pos) > taArrayItemP(a1, pos - 1)) &&
            (taArrayItemP(a1, pos - 1) < taArrayItemP(a1, pos - 2)))
        return (taUP);
    else if ((taArrayItemP(a1, pos) < taArrayItemP(a1, pos - 1)) &&
             (taArrayItemP(a1, pos - 1) > taArrayItemP(a1, pos - 2)))
        return (taDOWN);
    else
        return (taZERO);
}

int DLL_EXPORT taTurnFill(taArray *a1, taArray *a2, int start, int stop)
{
    int    pos;

    taArrayItemP(a2, start) = taZERO;
    taArrayItemP(a2, start + 1) = taZERO;
    for (pos = start + 2; pos <= stop && pos < a1->size && pos < a2->size; pos++)
        taArrayItemP(a2, pos) = taTurn(a1, pos);
    return (0);
}
