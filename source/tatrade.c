/* Technical Analysis Programmer's Toolkit - Copyright (C) 1997 KTA */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <float.h>
#include <ctype.h>
#include "ktakit.h"

/* Internal Functions & Macros - called only within TATRADE.C */

#define ROUNDUP(num,to) (num+to/2-fmod(num+to/2,to))
#define ROUNDDN(num,to) (num-fmod(num,to))
#define RNDUPPLUS(num,to) (ROUNDUP(num,to)+to)
#define RNDDNPLUS(num,to) (ROUNDDN(num,to)-to)

void    taInitCurrentParms(struct taOrderSystem *taOS);
int     taCreateInExec(struct taOrderSystem *taOS, int ordnum, char buysell, KFloat price);
int     taCloseInExec(struct taOrderSystem *taOS, int ordnum, char buysell, KFloat price);
int     taMatchInExec(struct taOrderSystem *taOS, int ordnum, char buysell, KFloat price);
int     taCreateOutExec(struct taOrderSystem *taOS, int ordnum, char buysell,
                        KFloat price, long inexecnum, long volume);
KFloat  taCalcProfit(struct taOrderSystem *taOS, long in, struct taExecution OutExec);
int     taUpdateCurrent(struct taOrderSystem *taOS);
int     taSortOrders(const void *elem1v, const void *elem2v);


int DLL_EXPORT taOrderSystemInit(struct taOrderSystem *taOS, char *systemname,
                                taBars *bars, int maxorders)
{
    taOS->Order = (struct taOrder *)taMalloc(maxorders * sizeof(struct taOrder));
    if(taOS->Order == NULL)
    {
        return (taErrMalloc);
    }
    taOS->InExec = (struct taExecution *)taMalloc(maxorders * sizeof(struct taExecution));
    if(taOS->InExec == NULL)
    {
        taFree(taOS->Order);
        return (taErrMalloc);
    }
    taOS->tradegraph = taNOARRAY;
    memset(taOS->Order, 0, maxorders * sizeof(struct taOrder));
    memset(taOS->InExec, 0, maxorders * sizeof(struct taExecution));
    taOS->OrderSize = maxorders;
    taOS->OrderCnt = 0;
    taOS->InExecSize = maxorders;
    taOS->InExecCnt = 0;
    memset((char *)&taOS->Long, 0, sizeof(struct taPerformance));
    memset((char *)&taOS->Short, 0, sizeof(struct taPerformance));
    memset((char *)&taOS->Net, 0, sizeof(struct taPerformance));
    taOS->Current.barnum = -1;
    taInitCurrentParms(taOS);
    strncpy(taOS->OrderParms.systemname, systemname, 80);
    taOS->OrderParms.systemname[79] = '\0';
    taOS->bars = *bars;
    taOS->OrderParms.ticksize = .001;
    taOS->OrderParms.valueoftick = .001;
    taOS->OrderParms.slippage = 0;
    taOS->OrderParms.commission = 0;
    taOS->OrderParms.margin = 100;
    taOS->OrderParms.ordertimedelay = 0;
    taOS->OrderParms.reversal = 1;
    taOS->OrderParms.bestprice = 0;
    strncpy(taOS->OrderParms.ttreport, "tradedtl.txt", 80);
    taOS->OrderParms.ttreport[79] = '\0';
    return (0);
}


void DLL_EXPORT taOrderSystemReset(struct taOrderSystem *taOS)
{
    memset(taOS->Order, 0, taOS->OrderSize * sizeof(struct taOrder));
    memset(taOS->InExec, 0, taOS->InExecSize * sizeof(struct taExecution));
    taOS->OrderCnt = 0;
    taOS->InExecCnt = 0;
    memset((char *)&taOS->Long, 0, sizeof(struct taPerformance));
    memset((char *)&taOS->Short, 0, sizeof(struct taPerformance));
    memset((char *)&taOS->Net, 0, sizeof(struct taPerformance));
    taOS->Current.barnum = -1;
    taInitCurrentParms(taOS);
    return;
}


void DLL_EXPORT taOrderSystemClose(struct taOrderSystem *taOS)
{
    taFree(taOS->Order);
    taFree(taOS->InExec);
    return;
}


int DLL_EXPORT taProcessOrders(struct taOrderSystem *taOS)
{
    int    i1 = 0, i2 = 0, sw = 0, compress = 0;
    int    buyopen = 0, sellopen = 0, buyclose = 0, sellclose = 0;
    FILE    *outptr = NULL;

    taOS->Current.barnum++;
    if (taOS->Current.barnum == 0)  /* first time through */
    {
        if (taOS->OrderParms.ttreport[0] != '\0')
        {
#ifndef _WINDOWS
            if (!stricmp(taOS->OrderParms.ttreport, "STDOUT")) outptr = stdout;
            else
#endif
                if ((outptr = fopen(taOS->OrderParms.ttreport, "wt")) == NULL)
                    return (taErrFileOpen);
            fprintf(outptr, " Trade Summary  %s\n\n", taOS->OrderParms.systemname);
            fprintf(outptr, " Date     Time     Bar#   Name                    Price   Volume      Profit\n");
            fprintf(outptr, " ---------------------------------------------------------------------------\n");
#ifndef _WINDOWS
            if (stricmp(taOS->OrderParms.ttreport, "STDOUT"))
#endif
                fclose(outptr);
        }
        return (1);
    }
    if (taOS->Current.barnum >= taBarSize(taOS->bars)) return (0);

    if (taOS->OrderCnt)
        qsort(taOS->Order, taOS->OrderCnt, sizeof(struct taOrder), taSortOrders);

    for (i1 = 0; i1 < taOS->OrderCnt; i1++)
    {
        if (taOS->Order[i1].deletesw == 1) continue;

        if (taOS->Order[i1].date > taArrayItem(taOS->bars.dt, taOS->Current.barnum))
            continue;

        if ((unsigned)taOS->Order[i1].time + (unsigned)taOS->OrderParms.ordertimedelay
            > (unsigned)taArrayItem(taOS->bars.ti, taOS->Current.barnum))
            continue;

        switch (taOS->Order[i1].type)
        {
        case taOPEN:
            switch (taOS->Order[i1].atind)
            {
            case taMARKET:
                switch (taOS->Order[i1].buysell)
                {
                case taBUY:
                    if (!buyopen)
                    {
                        /* create trade inexec with open */
                        taCreateInExec(taOS, i1, taBUY, taArrayItem(taOS->bars.op, taOS->Current.barnum));
                        buyopen = 1;
                    }
                    break;

                case taSELL:
                    if (!sellopen)
                    {
                        /* create trade inexec with open */
                        taCreateInExec(taOS, i1, taSELL, taArrayItem(taOS->bars.op, taOS->Current.barnum));
                        sellopen = 1;
                    }
                    break;
                }
                break;                      /* Market Break */

            case taLIMIT:                  /* Limit */
                switch (taOS->Order[i1].buysell)
                {
                case taBUY:
                    if (!buyopen &&
                        taArrayItem(taOS->bars.hi, taOS->Current.barnum) >= taOS->Order[i1].reqprice)
                    {
                        if (taOS->OrderParms.bestprice &&
                                taOS->Order[i1].reqprice <= taArrayItem(taOS->bars.op, taOS->Current.barnum))
                        {
                            taCreateInExec(taOS, i1, taBUY, taArrayItem(taOS->bars.op, taOS->Current.barnum));
                        }
                        else if (taOS->Order[i1].reqprice <= taArrayItem(taOS->bars.lo, taOS->Current.barnum))
                        {
                            /* create trade inexec with low */
                            taCreateInExec(taOS, i1, taBUY, taArrayItem(taOS->bars.lo, taOS->Current.barnum));
                        }
                        else
                        {
                            /* create trade inexec with taOS->Order[i1].reqprice + taOS->OrderParms.ticksize */
                            taCreateInExec(taOS, i1, taBUY, RNDUPPLUS(taOS->Order[i1].reqprice, taOS->OrderParms.ticksize));
                        }
                        buyopen = 1;
                    }
                    break;
                case taSELL:
                    if (!sellopen && taArrayItem(taOS->bars.lo, taOS->Current.barnum) <= taOS->Order[i1].reqprice)
                    {
                        if (taOS->OrderParms.bestprice &&
                                taOS->Order[i1].reqprice >= taArrayItem(taOS->bars.op, taOS->Current.barnum))
                        {
                            taCreateInExec(taOS, i1, taSELL, taArrayItem(taOS->bars.op, taOS->Current.barnum));
                        }
                        else if (taOS->Order[i1].reqprice >= taArrayItem(taOS->bars.hi, taOS->Current.barnum))
                        {
                            /* create trade inexec with high */
                            taCreateInExec(taOS, i1, taSELL, taArrayItem(taOS->bars.hi, taOS->Current.barnum));
                        }
                        else
                        {
                            /* create trade inexec with taOS->Order[i1].reqprice - taOS->OrderParms.ticksize */
                            taCreateInExec(taOS, i1, taSELL, RNDDNPLUS(taOS->Order[i1].reqprice, taOS->OrderParms.ticksize));
                        }
                        sellopen = 1;
                    }
                    break;
                }
                break;

            case taSTOP:
                switch (taOS->Order[i1].buysell)
                {
                case taBUY:
                    if (!buyopen && taArrayItem(taOS->bars.lo, taOS->Current.barnum) <= taOS->Order[i1].reqprice)
                    {
                        if (taOS->OrderParms.bestprice &&
                                taOS->Order[i1].reqprice >= taArrayItem(taOS->bars.op, taOS->Current.barnum))
                            taCreateInExec(taOS, i1, taBUY, taArrayItem(taOS->bars.op, taOS->Current.barnum));
                        else if (taOS->Order[i1].reqprice >= taArrayItem(taOS->bars.hi, taOS->Current.barnum))
                        {
                            /* create trade inexec with high */
                            taCreateInExec(taOS, i1, taBUY, taArrayItem(taOS->bars.hi, taOS->Current.barnum));
                        }
                        else
                        {
                            /* create trade inexec with taOS->Order[i1].reqprice - taOS->OrderParms.ticksize */
                            taCreateInExec(taOS, i1, taBUY, RNDDNPLUS(taOS->Order[i1].reqprice, taOS->OrderParms.ticksize));
                        }
                        buyopen = 1;
                    }
                    break;
                case taSELL:
                    if (!sellopen && taArrayItem(taOS->bars.hi, taOS->Current.barnum) >= taOS->Order[i1].reqprice)
                    {
                        if (taOS->OrderParms.bestprice &&
                                taOS->Order[i1].reqprice <= taArrayItem(taOS->bars.op, taOS->Current.barnum))
                            taCreateInExec(taOS, i1, taSELL, taArrayItem(taOS->bars.op, taOS->Current.barnum));
                        else if (taOS->Order[i1].reqprice <= taArrayItem(taOS->bars.lo, taOS->Current.barnum))
                        {
                            /* create trade inexec with low */
                            taCreateInExec(taOS, i1, taSELL, taArrayItem(taOS->bars.lo, taOS->Current.barnum));
                        }
                        else
                        {
                            /* create trade inexec with taOS->Order[i1].reqprice + taOS->OrderParms.ticksize */
                            taCreateInExec(taOS, i1, taSELL, RNDUPPLUS(taOS->Order[i1].reqprice, taOS->OrderParms.ticksize));
                        }
                        sellopen = 1;
                    }
                    break;
                }
                break;
            }
            break;  /* Open break */


        case taCLOSE:
            switch (taOS->Order[i1].atind)
            {
            case taMARKET:
                switch (taOS->Order[i1].buysell)
                {
                case taBUY:
                    /* update trade outexec with open */
                    taMatchInExec(taOS, i1, taBUY, taArrayItem(taOS->bars.op, taOS->Current.barnum));
                    break;
                case taSELL:
                    /* update trade outexec with open */
                    taMatchInExec(taOS, i1, taSELL, taArrayItem(taOS->bars.op, taOS->Current.barnum));
                    break;
                }
                break;

            case taLIMIT:                  /* Stop */
                switch (taOS->Order[i1].buysell)
                {
                case taBUY:
                    if (taArrayItem(taOS->bars.lo, taOS->Current.barnum) <= taOS->Order[i1].reqprice)
                    {
                        if (taOS->OrderParms.bestprice &&
                                taOS->Order[i1].reqprice >= taArrayItem(taOS->bars.op, taOS->Current.barnum))
                            taMatchInExec(taOS, i1, taBUY, taArrayItem(taOS->bars.op, taOS->Current.barnum));
                        else if (taOS->Order[i1].reqprice >= taArrayItem(taOS->bars.hi, taOS->Current.barnum))
                        {
                            /* update trade outexec with high */
                            taMatchInExec(taOS, i1, taBUY, taArrayItem(taOS->bars.hi, taOS->Current.barnum));
                        }
                        else
                        {
                            /* update trade outexec with taOS->Order[i1].reqprice - taOS->OrderParms.ticksize */
                            taMatchInExec(taOS, i1, taBUY, ROUNDDN(taOS->Order[i1].reqprice, taOS->OrderParms.ticksize));
                        }
                    }
                    break;

                case taSELL:
                    if (taArrayItem(taOS->bars.hi, taOS->Current.barnum) >= taOS->Order[i1].reqprice)
                    {
                        if (taOS->OrderParms.bestprice &&
                                taOS->Order[i1].reqprice <= taArrayItem(taOS->bars.op, taOS->Current.barnum))
                            taMatchInExec(taOS, i1, taSELL, taArrayItem(taOS->bars.op, taOS->Current.barnum));
                        else if (taOS->Order[i1].reqprice <= taArrayItem(taOS->bars.lo, taOS->Current.barnum))
                        {
                            /* update trade outexec with low */
                            taMatchInExec(taOS, i1, taSELL, taArrayItem(taOS->bars.lo, taOS->Current.barnum));
                        }
                        else
                        {
                            /* update trade outexec with taOS->Order[i1].reqprice + taOS->OrderParms.ticksize */
                            taMatchInExec(taOS, i1, taSELL, ROUNDUP(taOS->Order[i1].reqprice, taOS->OrderParms.ticksize));
                        }
                    }
                    break;
                }
                break;

            case taSTOP:                   /* At specific Price or Better Price */
                switch (taOS->Order[i1].buysell)
                {
                case taBUY:
                    if (taArrayItem(taOS->bars.hi, taOS->Current.barnum) >= taOS->Order[i1].reqprice)
                    {
                        if (taOS->OrderParms.bestprice && taOS->Order[i1].reqprice <=
                        taArrayItem(taOS->bars.op, taOS->Current.barnum))
                            taMatchInExec(taOS, i1, taBUY, taArrayItem(taOS->bars.op, taOS->Current.barnum));
                        else if (taOS->Order[i1].reqprice <= taArrayItem(taOS->bars.lo, taOS->Current.barnum))
                        {
                            /* update trade outexec with low */
                            taMatchInExec(taOS, i1, taBUY, taArrayItem(taOS->bars.lo, taOS->Current.barnum));
                        }
                        else
                        {
                            /* update trade outexec with taOS->Order[i1].reqprice + taOS->OrderParms.ticksize */
                            taMatchInExec(taOS, i1, taBUY, ROUNDUP(taOS->Order[i1].reqprice, taOS->OrderParms.ticksize));
                        }
                    }
                    break;
                case taSELL:
                    if (taArrayItem(taOS->bars.lo, taOS->Current.barnum) <= taOS->Order[i1].reqprice)
                    {
                        if (taOS->OrderParms.bestprice && taOS->Order[i1].reqprice >=
                            taArrayItem(taOS->bars.op, taOS->Current.barnum))
                        {
                            taMatchInExec(taOS, i1, taSELL, taArrayItem(taOS->bars.op, taOS->Current.barnum));
                        }
                        else if (taOS->Order[i1].reqprice >= taArrayItem(taOS->bars.hi, taOS->Current.barnum))
                        {
                            /* update trade outexec with high */
                            taMatchInExec(taOS, i1, taSELL, taArrayItem(taOS->bars.hi, taOS->Current.barnum));
                        }
                        else
                        {
                            /* update trade outexec with taOS->Order[i1].reqprice - taOS->OrderParms.ticksize */
                            taMatchInExec(taOS, i1, taSELL, ROUNDDN(taOS->Order[i1].reqprice, taOS->OrderParms.ticksize));
                        }
                    }
                    break;
                }
                break;
            }
            break;
        }                             /* switch on Type */
    }                              /* scan of orders */

    taUpdateCurrent(taOS);

    /* Compress Order Stack */
    for (i1 = 0, i2 = 0; i1 <= taOS->OrderCnt; i1++)
    {
        if (i1 < taOS->OrderCnt &&
                (taOS->Order[i1].timeinforce == taDAY ||
                 taOS->Order[i1].deletesw == 1 ||
                 (buyopen && taOS->Order[i1].type == taOPEN &&
                  taOS->Order[i1].buysell == taBUY) ||
                 (sellopen && taOS->Order[i1].type == taOPEN &&
                  taOS->Order[i1].buysell == taSELL)))
            compress++;
        else
        {
            if (compress)
            {
                memmove((char *) &taOS->Order[i2], (char *) &taOS->Order[i1],
                        (taOS->OrderCnt - i1) * sizeof(struct taOrder));
                taOS->OrderCnt -= compress;
                i1 -= compress;
                compress = 0;
            }
            i2 = i1;
        }
    }

    /* Compress InExec Stack */
    for (i1 = 0, i2 = 0; i1 <= taOS->InExecCnt; i1++)
    {
        if (i1 < taOS->InExecCnt && taOS->InExec[i1].remvolume == 0) compress++;
        else
        {
            if (compress)
            {
                memmove((char *) &taOS->InExec[i2], (char *) &taOS->InExec[i1],
                        (taOS->InExecCnt - i1) * sizeof(struct taExecution));
                taOS->InExecCnt -= compress;
                i1 -= compress;
                compress = 0;
            }
            i2 = i1;
        }
    }
    return (1);
}


int DLL_EXPORT taBuyToOpen(struct taOrderSystem *taOS, char *name, long volume,
                            KFloat price, int atind, int timeinforce)
{
    if (taOS->OrderCnt >= taOS->OrderSize)
        return (taErrMalloc);
    strncpy(taOS->Order[taOS->OrderCnt].name, name, 20);
    taOS->Order[taOS->OrderCnt].name[19] = '\0';
    strcpy(taOS->Order[taOS->OrderCnt].fromname, "\0");
    taOS->Order[taOS->OrderCnt].type = taOPEN;
    taOS->Order[taOS->OrderCnt].buysell = taBUY;
    taOS->Order[taOS->OrderCnt].volume = volume;
    if (atind == taMARKET)
        taOS->Order[taOS->OrderCnt].reqprice = 0;
    else
        taOS->Order[taOS->OrderCnt].reqprice = price;
    taOS->Order[taOS->OrderCnt].atind = atind;
    taOS->Order[taOS->OrderCnt].timeinforce = timeinforce;
    taOS->Order[taOS->OrderCnt].date = taArrayItem(taOS->bars.dt, taOS->Current.barnum);
    taOS->Order[taOS->OrderCnt].time = taArrayItem(taOS->bars.ti, taOS->Current.barnum);
    taOS->Order[taOS->OrderCnt].deletesw = 0;
    taOS->OrderCnt++;
    return (0);
}


int DLL_EXPORT taSellToOpen(struct taOrderSystem *taOS, char *name, long volume,
                            KFloat price, int atind, int timeinforce)
{
    if (taOS->OrderCnt >= taOS->OrderSize)
        return (taErrMalloc);
    strncpy(taOS->Order[taOS->OrderCnt].name, name, 20);
    taOS->Order[taOS->OrderCnt].name[19] = '\0';
    strcpy(taOS->Order[taOS->OrderCnt].fromname, "\0");
    taOS->Order[taOS->OrderCnt].type = taOPEN;
    taOS->Order[taOS->OrderCnt].buysell = taSELL;
    taOS->Order[taOS->OrderCnt].volume = volume;
    if (atind == taMARKET)
        taOS->Order[taOS->OrderCnt].reqprice = FLT_MAX;
    else
        taOS->Order[taOS->OrderCnt].reqprice = price;
    taOS->Order[taOS->OrderCnt].atind = atind;
    taOS->Order[taOS->OrderCnt].timeinforce = timeinforce;
    taOS->Order[taOS->OrderCnt].date = taArrayItem(taOS->bars.dt, taOS->Current.barnum);
    taOS->Order[taOS->OrderCnt].time = taArrayItem(taOS->bars.ti, taOS->Current.barnum);
    taOS->Order[taOS->OrderCnt].deletesw = 0;
    taOS->OrderCnt++;
    return (0);
}


int DLL_EXPORT taSellToClose(struct taOrderSystem *taOS, char *name, char *fromname,
                            long volume, KFloat price, int atind, int timeinforce)
{
    if (taOS->OrderCnt >= taOS->OrderSize)
        return (taErrMalloc);

    strncpy(taOS->Order[taOS->OrderCnt].name, name, 20);
    taOS->Order[taOS->OrderCnt].name[19] = '\0';
    strncpy(taOS->Order[taOS->OrderCnt].fromname, fromname, 20);
    taOS->Order[taOS->OrderCnt].fromname[19] = '\0';
    taOS->Order[taOS->OrderCnt].type = taCLOSE;
    taOS->Order[taOS->OrderCnt].buysell = taSELL;
    taOS->Order[taOS->OrderCnt].volume = volume;
    if (atind == taMARKET) taOS->Order[taOS->OrderCnt].reqprice = FLT_MAX;
    else taOS->Order[taOS->OrderCnt].reqprice = price;
    taOS->Order[taOS->OrderCnt].atind = atind;
    taOS->Order[taOS->OrderCnt].timeinforce = timeinforce;
    taOS->Order[taOS->OrderCnt].date = taArrayItem(taOS->bars.dt, taOS->Current.barnum);
    taOS->Order[taOS->OrderCnt].time = taArrayItem(taOS->bars.ti, taOS->Current.barnum);
    taOS->Order[taOS->OrderCnt].deletesw = 0;
    taOS->OrderCnt++;
    return (0);
}


int DLL_EXPORT taBuyToClose(struct taOrderSystem *taOS, char *name, char *fromname,
                        long volume, KFloat price, int atind, int timeinforce)       /* TS ExitShort */
{
    if (taOS->OrderCnt >= taOS->OrderSize)
        return (taErrMalloc);

    strncpy(taOS->Order[taOS->OrderCnt].name, name, 20);
    taOS->Order[taOS->OrderCnt].name[19] = '\0';
    strncpy(taOS->Order[taOS->OrderCnt].fromname, fromname, 20);
    taOS->Order[taOS->OrderCnt].fromname[19] = '\0';
    taOS->Order[taOS->OrderCnt].type = taCLOSE;
    taOS->Order[taOS->OrderCnt].buysell = taBUY;
    taOS->Order[taOS->OrderCnt].volume = volume;
    if (atind == taMARKET) taOS->Order[taOS->OrderCnt].reqprice = 0;
    else taOS->Order[taOS->OrderCnt].reqprice = price;
    taOS->Order[taOS->OrderCnt].atind = atind;
    taOS->Order[taOS->OrderCnt].timeinforce = timeinforce;
    taOS->Order[taOS->OrderCnt].date = taArrayItem(taOS->bars.dt, taOS->Current.barnum);
    taOS->Order[taOS->OrderCnt].time = taArrayItem(taOS->bars.ti, taOS->Current.barnum);
    taOS->Order[taOS->OrderCnt].deletesw = 0;
    taOS->OrderCnt++;
    return (0);
}


int DLL_EXPORT taCancelOrder(struct taOrderSystem *taOS, char *name)
{
    int    i1 = 0;

    for (i1 = 0; i1 <= taOS->OrderCnt; i1++)
        if (!name[0] || !strcmp(taOS->Order[i1].name, name))
            taOS->Order[i1].deletesw = 1;

    return (0);
}


int DLL_EXPORT taPrintOrders(struct taOrderSystem *taOS, char *outfile, int append)
{
    int    i1 = 0;
    KFloat   f1;
    char     dt[9];
    char     ti[9];
    FILE    *outptr = NULL;

    if (taOS->OrderCnt == 0) return (0);

#ifndef _WINDOWS
    if (!stricmp(outfile, "STDOUT")) outptr = stdout;
    else
#endif
    {
        if (append)
        {
            if ((outptr = fopen(outfile, "at")) == NULL) return (-4);
        }
        else
        {
            if ((outptr = fopen(outfile, "wt")) == NULL) return (-4);
        }
    }

    fprintf(outptr, "\nOpen Orders\n");

    for (i1 = 0; i1 < taOS->OrderCnt && i1 < taOS->OrderSize; i1++)
    {
        if (taOS->Order[i1].reqprice == FLT_MAX) f1 = 0;
        else f1 = taOS->Order[i1].reqprice;
        taJulianToDateTxt(taOS->Order[i1].date, DTFORMAT_MM_DD_YY, "/", dt);
        taJulianToTimeTxt(taOS->Order[i1].date, TIFORMAT_HH_MM_SS, ":", ti);
        fprintf(outptr, "%4i %s %s %-20s %c %c %c %c %8li %4li.%03li\n",
                i1, dt, ti,
                taOS->Order[i1].name, taOS->Order[i1].type,
                taOS->Order[i1].buysell, taOS->Order[i1].atind,
                taOS->Order[i1].timeinforce, taOS->Order[i1].volume, (long)f1, taDecimals(f1,3));
    }
#ifndef _WINDOWS
    if (stricmp(outfile, "STDOUT"))
#endif
        fclose(outptr);
    return (0);
}


int DLL_EXPORT taProfitRpt(struct taOrderSystem *taOS, char *outfile, int append)
{
    FILE    *outptr = NULL;
    char    title[80] = "\0";
    char    buffer[16];

#ifndef _WINDOWS
    if (!stricmp(outfile, "STDOUT")) outptr = stdout;
    else
#endif
    {
        if (append)
        {
            if ((outptr = fopen(outfile, "at")) == NULL) return (-4);
        }
        else
        {
            if ((outptr = fopen(outfile, "wt")) == NULL) return (-4);
        }
    }
    fprintf(outptr, "--------------------------------------------------------------\n");

    taJulianToDateTxt(taArrayItem(taOS->bars.dt, 0), DTFORMAT_MM_DD_YY, "/", buffer);
    fprintf(outptr, "%s  %s - ", taOS->OrderParms.systemname, buffer);
    taJulianToDateTxt(taArrayItem(taOS->bars.dt, taOS->bars.size - 1), DTFORMAT_MM_DD_YY, "/", buffer);
    fprintf(outptr, "%s  %s\n\n", buffer, title);
    fprintf(outptr,"                                 NET         LONG        SHORT\n");
    fprintf(outptr, "  Total net profit       %8li.%02li  %8li.%02li  %8li.%02li\n",
            (long)taOS->Net.netprofit, taDecimals(taOS->Net.netprofit, 2),
            (long)taOS->Long.netprofit, taDecimals(taOS->Long.netprofit, 2),
            (long)taOS->Short.netprofit, taDecimals(taOS->Short.netprofit, 2));
    fprintf(outptr, "  Gross profit           %8li.%02li  %8li.%02li  %8li.%02li\n",
            (long)taOS->Net.grossprofit, taDecimals(taOS->Net.grossprofit, 2),
            (long)taOS->Long.grossprofit, taDecimals(taOS->Long.grossprofit, 2),
            (long)taOS->Short.grossprofit, taDecimals(taOS->Short.grossprofit, 2));
    fprintf(outptr, "  Gross loss             %8li.%02li  %8li.%02li  %8li.%02li\n",
            (long)taOS->Net.grossloss, taDecimals(taOS->Net.grossloss, 2),
            (long)taOS->Long.grossloss, taDecimals(taOS->Long.grossloss, 2),
            (long)taOS->Short.grossloss, taDecimals(taOS->Short.grossloss, 2));
    fprintf(outptr, "  Total # of trades         %5ld        %5ld        %5ld \n",
            taOS->Net.trades, taOS->Long.trades, taOS->Short.trades);
    fprintf(outptr, "  Percent Profitable          %3li          %3li          %3li\n",
            (long)taOS->Net.pctprofitable, (long)taOS->Long.pctprofitable,(long)taOS->Short.pctprofitable);
    fprintf(outptr, "  Number winning trades     %5d        %5d        %5d \n",
            taOS->Net.win, taOS->Long.win, taOS->Short.win);
    fprintf(outptr, "  Number losing trades      %5d        %5d        %5d \n",
            taOS->Net.loss, taOS->Long.loss, taOS->Short.loss);
    fprintf(outptr, "  Largest winning trade  %8li.%02li  %8li.%02li  %8li.%02li\n",
            (long)taOS->Net.largestwin, taDecimals(taOS->Net.largestwin, 2),
            (long)taOS->Long.largestwin, taDecimals(taOS->Long.largestwin, 2),
            (long)taOS->Short.largestwin, taDecimals(taOS->Short.largestwin, 2));
    fprintf(outptr, "  Largest losing trade   %8li.%02li  %8li.%02li  %8li.%02li\n",
            (long)taOS->Net.largestloss, taDecimals(taOS->Net.largestloss, 2),
            (long)taOS->Long.largestloss, taDecimals(taOS->Long.largestloss, 2),
            (long)taOS->Short.largestloss, taDecimals(taOS->Short.largestloss, 2));
    fprintf(outptr, "  Average winning trade  %8li.%02li  %8li.%02li  %8li.%02li\n",
            (long)taOS->Net.avgwin, taDecimals(taOS->Net.avgwin, 2),
            (long)taOS->Long.avgwin, taDecimals(taOS->Long.avgwin, 2),
            (long)taOS->Short.avgwin, taDecimals(taOS->Short.avgwin, 2));
    fprintf(outptr, "  Average losing trade   %8li.%02li  %8li.%02li  %8li.%02li\n",
            (long)taOS->Net.avgloss, taDecimals(taOS->Net.avgloss, 2),
            (long)taOS->Long.avgloss, taDecimals(taOS->Long.avgloss, 2),
            (long)taOS->Short.avgloss, taDecimals(taOS->Short.avgloss, 2));
    fprintf(outptr, "  Ratio avg win/avg loss %8li.%02li  %8li.%02li  %8li.%02li\n",
            (long)taOS->Net.avgwintoavgloss, taDecimals(taOS->Net.avgwintoavgloss, 2),
            (long)taOS->Long.avgwintoavgloss, taDecimals(taOS->Long.avgwintoavgloss, 2),
            (long)taOS->Short.avgwintoavgloss, taDecimals(taOS->Short.avgwintoavgloss, 2));
    fprintf(outptr, "  Avg trade(win & loss)  %8li.%02li  %8li.%02li  %8li.%02li\n",
            (long)taOS->Net.avgtrade, taDecimals(taOS->Net.avgtrade, 2),
            (long)taOS->Long.avgtrade, taDecimals(taOS->Long.avgtrade, 2),
            (long)taOS->Short.avgtrade, taDecimals(taOS->Short.avgtrade, 2));
    fprintf(outptr, "  Max consec. winners       %5d        %5d        %5d \n",
            taOS->Net.maxconwin, taOS->Long.maxconwin, taOS->Short.maxconwin);
    fprintf(outptr, "  Max consec. losers        %5d        %5d        %5d \n",
            taOS->Net.maxconloss, taOS->Long.maxconloss, taOS->Short.maxconloss);
    fprintf(outptr, "  Avg # bars in winners     %5li        %5li        %5li\n",
            (long)taOS->Net.avgwinstreak,(long)taOS->Long.avgwinstreak,(long)taOS->Short.avgwinstreak);
    fprintf(outptr, "  Avg # bars in losers      %5li        %5li        %5li\n",
            (long)taOS->Net.avglosstreak,(long)taOS->Long.avglosstreak,(long)taOS->Short.avglosstreak);
    fprintf(outptr, "  Max intraday drawdown  %8li.%02li  %8li.%02li  %8li.%02li\n",
            (long)taOS->Net.maxdrawdown, taDecimals(taOS->Net.maxdrawdown, 2),
            (long)taOS->Long.maxdrawdown, taDecimals(taOS->Long.maxdrawdown, 2),
            (long)taOS->Short.maxdrawdown, taDecimals(taOS->Short.maxdrawdown, 2));
    fprintf(outptr, "  Profit factor          %8li.%02li  %8li.%02li  %8li.%02li\n",
            (long)taOS->Net.profitfactor, taDecimals(taOS->Net.profitfactor, 2),
            (long)taOS->Long.profitfactor, taDecimals(taOS->Long.profitfactor, 2),
            (long)taOS->Short.profitfactor, taDecimals(taOS->Short.profitfactor, 2));
    fprintf(outptr, "  Max # contracts held  %9li    %9li     %9li\n",
            (long)taOS->Net.maxcontractsheld,(long)taOS->Long.maxcontractsheld,(long)taOS->Short.maxcontractsheld);
    fprintf(outptr, "  Account size required  %8li.%02li  %8li.%02li  %8li.%02li\n",
            (long)taOS->Net.acctsizereqd, taDecimals(taOS->Net.acctsizereqd, 2),
            (long)taOS->Long.acctsizereqd, taDecimals(taOS->Long.acctsizereqd, 2),
            (long)taOS->Short.acctsizereqd, taDecimals(taOS->Short.acctsizereqd, 2));
    fprintf(outptr, "  Return on account      %8li.%02li%%  %8li.%02li%%  %8li.%02li%%\n",
            (long)taOS->Net.rtnonacct, taDecimals(taOS->Net.rtnonacct, 2),
            (long)taOS->Long.rtnonacct, taDecimals(taOS->Long.rtnonacct, 2),
            (long)taOS->Short.rtnonacct, taDecimals(taOS->Short.rtnonacct, 2));
    fprintf(outptr, "  PRR                    %8li.%02li  %8li.%02li  %8li.%02li\n",
            (long)taOS->Net.prr, taDecimals(taOS->Net.prr, 2),
            (long)taOS->Long.prr, taDecimals(taOS->Long.prr, 2),
            (long)taOS->Short.prr, taDecimals(taOS->Short.prr, 2));
    fprintf(outptr, "  Open Position P/L      %8li.%02li  %8li.%02li  %8li.%02li\n",
            (long)taOS->Net.openpospl, taDecimals(taOS->Net.openpospl, 2),
            (long)taOS->Long.openpospl, taDecimals(taOS->Long.openpospl, 2),
            (long)taOS->Short.openpospl, taDecimals(taOS->Short.openpospl, 2));

#ifndef _WINDOWS
    if (stricmp(outfile, "STDOUT"))
#endif
        fclose(outptr);
    return (0);
}


int DLL_EXPORT taProfitRpt1(struct taOrderSystem *taOS, char *outfile, int append,
                        char *heading, char *fields)
{
    int    i1;
    int    i2;
    int    fldpos = 0;
    FILE    *outptr = NULL;
    struct   taPerformance perf;
    char     fldname[4];
    char    *names[23] = {"NP", "GP", "GL", "NT", "PP", "NW", "NL", "LW",
                          "LL", "AW", "AL", "WL", "AT", "MW", "ML", "WS",
                          "LS", "MD", "PF", "MC", "AS", "RA", "PR"
                         };

#ifndef _WINDOWS
    if (!stricmp(outfile, "STDOUT")) outptr = stdout;
    else
#endif
    {
        if (append)
        {
            if ((outptr = fopen(outfile, "at")) == NULL) return (taErrFileOpen);
        }
        else
        {
            if ((outptr = fopen(outfile, "wt")) == NULL) return (taErrFileOpen);
        }
    }
    fprintf(outptr, heading);
    fldname[3] = '\0';
    for (i1 = 0; fields[i1]; i1++)
    {
        if (isalpha(fields[i1])) fldname[fldpos++] = fields[i1];
        else fprintf(outptr, "%c", fields[i1]);
        if (fldpos == 3)
        {
            fldpos = 0;
            if (toupper(fldname[0]) == 'L') perf = taOS->Long;
            else if (toupper(fldname[0]) == 'S') perf = taOS->Short;
            else perf = taOS->Net;
            for (i2 = 0; i2 < 23; i2++)
                if (!stricmp(fldname + 1, names[i2])) break;
            switch (i2)
            {
            case 0:   /* NP */
                fprintf(outptr, " %8li.%02li", (long)perf.netprofit, taDecimals(perf.netprofit, 2));
                break;
            case 1:   /* GP */
                fprintf(outptr, " %8li.%02li", (long)perf.grossprofit, taDecimals(perf.grossprofit, 2));
                break;
            case 2:   /* GL */
                fprintf(outptr, " %8li.%02li", (long)perf.grossloss, taDecimals(perf.grossloss, 2));
                break;
            case 3:   /* NT */
                fprintf(outptr, "    %5li.00", perf.trades);
                break;
            case 4:   /* PP */
                fprintf(outptr, " %8li.%02li", (long)perf.pctprofitable, taDecimals(perf.pctprofitable, 2));
                break;
            case 5:   /* NW */
                fprintf(outptr, "    %5i.00", perf.win);
                break;
            case 6:   /* NL */
                fprintf(outptr, "    %5i.00", perf.loss);
                break;
            case 7:   /* LW */
                fprintf(outptr, " %8li.%02li", (long)perf.largestwin, taDecimals(perf.largestwin, 2));
                break;
            case 8:   /* LL */
                fprintf(outptr, " %8li.%02li", (long)perf.largestloss, taDecimals(perf.largestloss, 2));
                break;
            case 9:   /* AW */
                fprintf(outptr, " %8li.%02li", (long)perf.avgwin, taDecimals(perf.avgwin, 2));
                break;
            case 10:  /* AL */
                fprintf(outptr, " %8li.%02li", (long)perf.avgloss, taDecimals(perf.avgloss, 2));
                break;
            case 11:  /* WL */
                fprintf(outptr, " %8li.%02li", (long)perf.avgwintoavgloss, taDecimals(perf.avgwintoavgloss, 2));
                break;
            case 12:  /* AT */
                fprintf(outptr, " %8li.%02li", (long)perf.avgtrade, taDecimals(perf.avgtrade, 2));
                break;
            case 13:  /* MW */
                fprintf(outptr, "    %5i.00", perf.maxconwin);
                break;
            case 14:  /* ML */
                fprintf(outptr, "    %5i.00", perf.maxconloss);
                break;
            case 15:  /* WS */
                fprintf(outptr, " %8li.%02li", (long)perf.avgwinstreak, taDecimals(perf.avgwinstreak, 2));
                break;
            case 16:  /* LS */
                fprintf(outptr, " %8li.%02li", (long)perf.avglosstreak, taDecimals(perf.avglosstreak, 2));
                break;
            case 17:  /* MD */
                fprintf(outptr, " %8li.%02li", (long)perf.maxdrawdown, taDecimals(perf.maxdrawdown, 2));
                break;
            case 18:  /* PF */
                fprintf(outptr, " %8li.%02li", (long)perf.profitfactor, taDecimals(perf.profitfactor, 2));
                break;
            case 19:  /* MC */
                fprintf(outptr, " %8li.%02li", (long)perf.maxcontractsheld, taDecimals(perf.maxcontractsheld, 2));
                break;
            case 20:  /* AS */
                fprintf(outptr, " %8li.%02li", (long)perf.acctsizereqd, taDecimals(perf.acctsizereqd, 2));
                break;
            case 21:  /* RA */
                fprintf(outptr, " %8li.%02li", (long)perf.rtnonacct, taDecimals(perf.rtnonacct, 2));
                break;
            case 22:  /* PR */
                fprintf(outptr, " %8li.%02li", (long)perf.prr, taDecimals(perf.prr, 2));
                break;
            default:
                fprintf(outptr, " %11s", fldname);
                break;
            }
        }
    }
    fprintf(outptr, "\n");

#ifndef _WINDOWS
    if (stricmp(outfile, "STDOUT"))
#endif
        fclose(outptr);
    return (0);
}


/* ---------------------- */
/* Optimization Functions */
/* ---------------------- */

long DLL_EXPORT taOptVarCount(struct taOptVar *vars, int numvars)
{
    int      i1;
    long     count = 1;

    for (i1 = 0; i1 < numvars; i1++)
    {
        if (vars[i1].incr == 0) vars[i1].count = 1;
        else vars[i1].count = (long)((vars[i1].to - vars[i1].from) / vars[i1].incr + 1);
        count *= vars[i1].count;
    }
    return(count);
}


int DLL_EXPORT taOptVarValue(struct taOptVar *vars, int numvars, long iteration)
{
    int      i1;
    long     count = 1, step;

    if (iteration > taOptVarCount(vars, numvars)) return (taErrInvalidParm);
    iteration--;
    for (i1 = 0; i1 < numvars; i1++)
    {
        step = (long)(iteration / count) % vars[i1].count;
        vars[i1].value = vars[i1].from + vars[i1].incr * step;
        count *= vars[i1].count;
    }
    return (0);
}


/* ----------------------------------------------------------- */
/* Internal Functions - called only by other TATRADE functions */
/* ----------------------------------------------------------- */

void     taInitCurrentParms(struct taOrderSystem *taOS)
{
    taOS->Current.position = 0;
    taOS->Current.longvol = 0;
    taOS->Current.shortvol = 0;
    taOS->Current.totalvolume = 0;
    taOS->Current.avgentryprice = 0;
    taOS->Current.longduration = 0;
    taOS->Current.shortduration = 0;
    taOS->Current.duration = 0;
    return;
}


int      taCreateInExec(struct taOrderSystem *taOS, int ordnum, char buysell, KFloat price)
{
    if (taOS->InExecCnt >= taOS->InExecSize) return (taErrMalloc);
    taOS->InExec[taOS->InExecCnt].barnum = taOS->Current.barnum;
    strcpy(taOS->InExec[taOS->InExecCnt].name, taOS->Order[ordnum].name);
    taOS->InExec[taOS->InExecCnt].buysell = buysell;
    taOS->InExec[taOS->InExecCnt].date = taArrayItem(taOS->bars.dt, taOS->Current.barnum);
    taOS->InExec[taOS->InExecCnt].time = taArrayItem(taOS->bars.ti, taOS->Current.barnum);
    taOS->InExec[taOS->InExecCnt].price = ROUNDDN(price, taOS->OrderParms.ticksize);
    taOS->InExec[taOS->InExecCnt].volume = taOS->Order[ordnum].volume;
    taOS->InExec[taOS->InExecCnt].remvolume = taOS->Order[ordnum].volume;
    taOS->Current.entrydate = taOS->InExec[taOS->InExecCnt].date;
    taOS->Current.entrytime = taOS->InExec[taOS->InExecCnt].time;
    taOS->InExecCnt++;
    /* Once we create an execution             */
    /* close all open inexec of opposite types */
    taCloseInExec(taOS, ordnum, buysell, price);
    return (0);
}


int      taCloseInExec(struct taOrderSystem *taOS, int ordnum, char buysell, KFloat price)
{

    int      i1 = 0;

    if (taOS->OrderParms.reversal == 0)
        return (0);
    for (i1 = 0; i1 < taOS->InExecCnt; i1++)
    {
        if (taOS->InExec[i1].remvolume && taOS->InExec[i1].buysell != buysell)
        {
            taCreateOutExec(taOS, ordnum, buysell, price, i1, taOS->InExec[i1].remvolume);
        }
    }
    return (0);
}


int      taMatchInExec(struct taOrderSystem *taOS, int ordnum, char buysell, KFloat price)
{
    int      i1 = 0;
    long     ordervolume;

    taOS->Order[ordnum].deletesw = 1;
    ordervolume = taOS->Order[ordnum].volume;
    for (i1 = 0; i1 < taOS->InExecCnt && ordervolume; i1++)
    {
        if (taOS->InExec[i1].buysell != buysell &&
                taOS->InExec[i1].remvolume &&
                (taOS->Order[ordnum].fromname[0] == '\0' ||
                 !strcmp(taOS->Order[ordnum].fromname, taOS->InExec[i1].name)))
        {
            if (ordervolume <= taOS->InExec[i1].remvolume)
            {
                taCreateOutExec(taOS, ordnum, buysell, price, i1, ordervolume);
                ordervolume = 0;
            }
            else
            {
                ordervolume -= taOS->InExec[i1].remvolume;
                taCreateOutExec(taOS, ordnum, buysell, price, i1, taOS->InExec[i1].remvolume);
            }
        }
    }
    return (0);
}

int      taCreateOutExec(struct taOrderSystem *taOS, int ordnum, char buysell, KFloat price,
                         long inexecnum, long volume)
{
    int    i1;
    KFloat   profit = 0;
    char     buffer[16], buffer2[16];
    FILE    *outptr = NULL;
    struct taExecution OutExec;

    OutExec.barnum = taOS->Current.barnum;
    strcpy(OutExec.name, taOS->Order[ordnum].name);
    OutExec.buysell = buysell;
    OutExec.date = taArrayItem(taOS->bars.dt, taOS->Current.barnum);
    OutExec.time = taArrayItem(taOS->bars.ti, taOS->Current.barnum);
    OutExec.price = ROUNDDN(price, taOS->OrderParms.ticksize);
    OutExec.volume = volume;
    profit = taCalcProfit(taOS, inexecnum, OutExec);

    /* write trade by trade */
    if (taOS->OrderParms.ttreport[0])
    {
#ifndef _WINDOWS
        if (!stricmp(taOS->OrderParms.ttreport, "STDOUT")) outptr = stdout;
        else
#endif
            if ((outptr = fopen(taOS->OrderParms.ttreport, "at")) == NULL)
                return (-1);

        taJulianToDateTxt(taOS->InExec[inexecnum].date, DTFORMAT_MM_DD_YY, "/", buffer);
        taJulianToTimeTxt(taOS->InExec[inexecnum].time, TIFORMAT_HH_MM_SS, ":", buffer2);
        fprintf(outptr, "\n %s %s %4i %c %-20s %4li.%03li %8li\n",
                buffer,
                buffer2,
                taOS->InExec[inexecnum].barnum,
                (taOS->InExec[inexecnum].buysell == taBUY)?'B':'S',
                taOS->InExec[inexecnum].name,
                (long)taOS->InExec[inexecnum].price, taDecimals(taOS->InExec[inexecnum].price, 3),
                taOS->InExec[inexecnum].volume);

        taJulianToDateTxt(OutExec.date, DTFORMAT_MM_DD_YY, "/", buffer);
        taJulianToTimeTxt(OutExec.time, TIFORMAT_HH_MM_SS, ":", buffer2);
        fprintf(outptr, " %s %s %4i %c %-20s %4li.%03li %8li %7li.%03li\n",
                buffer,
                buffer2,
                OutExec.barnum,
                (OutExec.buysell == taBUY)?'B':'S',
                OutExec.name,
                (long)OutExec.price, taDecimals(OutExec.price, 3),
                OutExec.volume,
                (long)profit, taDecimals(profit, 3));

#ifndef _WINDOWS
        if (stricmp(taOS->OrderParms.ttreport, "STDOUT"))
#endif
            fclose(outptr);
    }

    if (!taIsNoArray(taOS->tradegraph))
    {
        i1 = (int) taArrayItem(taOS->tradegraph, taOS->InExec[inexecnum].barnum);
        if (taOS->InExec[inexecnum].buysell == taBUY) i1 |= taBO;
        else i1 |= taSO;
        taArrayItem(taOS->tradegraph, taOS->InExec[inexecnum].barnum) = i1;

        i1 = (int) taArrayItem(taOS->tradegraph, taOS->Current.barnum);
        if (OutExec.buysell == taBUY) i1 |= taBC;
        else i1 |= taSC;
        taArrayItem(taOS->tradegraph, taOS->Current.barnum) = i1;
    }

    if (volume >= taOS->InExec[inexecnum].remvolume)
        taOS->InExec[inexecnum].remvolume = 0;
    else
        taOS->InExec[inexecnum].remvolume -= volume;
    return (0);
}


KFloat   taCalcProfit(struct taOrderSystem *taOS, long in, struct taExecution OutExec)
{
    KFloat profit = 0, price = 0, shortmaxdrawdown = 0, longmaxdrawdown = 0;
    KFloat f1 = 0, f2 = 0;
    KFloat high = 1, low = 1;

    taArrayStats(&taOS->bars.cl, &low, &high, NULL, NULL,
                 taOS->InExec[in].barnum, OutExec.barnum, 0);

    switch (taOS->InExec[in].buysell)
    {
    case taSELL:
        profit = (KFloat) OutExec.volume * (taOS->OrderParms.valueoftick *
                                            (taOS->InExec[in].price - OutExec.price) /
                                            taOS->OrderParms.ticksize - taOS->OrderParms.slippage) -
                 2 * taOS->OrderParms.commission;
        taOS->Short.trades++;
        if (profit > 0)
        {
            taOS->Short.win++;
            taOS->Short.winstreak++;
            taOS->Short.grossprofit += profit;
            if (profit > taOS->Short.largestwin) taOS->Short.largestwin = profit;
            taOS->Short.lossstreak = 0;
            taOS->Short.maxconwin = max(taOS->Short.maxconwin, taOS->Short.winstreak);
            taOS->Short.winbars += OutExec.barnum - taOS->InExec[in].barnum + 1;
        }
        if (profit < 0)
        {
            taOS->Short.loss++;
            taOS->Short.lossstreak++;
            taOS->Short.grossloss += profit;
            if (profit < taOS->Short.largestloss) taOS->Short.largestloss = profit;
            taOS->Short.winstreak = 0;
            taOS->Short.maxconloss = max(taOS->Short.maxconloss, taOS->Short.lossstreak);
            taOS->Short.lossbars += OutExec.barnum - taOS->InExec[in].barnum + 1;
        }
        taOS->Short.netprofit = taOS->Short.grossprofit +
                                taOS->Short.grossloss;
        if (taOS->Short.grossloss)
            taOS->Short.profitfactor = fabs(taOS->Short.grossprofit / taOS->Short.grossloss);
        shortmaxdrawdown= OutExec.volume * taOS->OrderParms.valueoftick *
                          (low - high) / taOS->OrderParms.ticksize;
        taOS->Short.maxdrawdown = min(taOS->Short.maxdrawdown, shortmaxdrawdown);

        if (taOS->OrderParms.margin)
        {
            if (taOS->OrderParms.margin == 100)
                taOS->Short.acctsizereqd = fabs(taOS->Short.maxdrawdown);
            else
                taOS->Short.acctsizereqd = fabs(taOS->Short.maxdrawdown / (taOS->OrderParms.margin / 100));
        }
        if (taOS->Short.acctsizereqd)
            taOS->Short.rtnonacct = 100 * taOS->Short.netprofit / taOS->Short.acctsizereqd;

        break;

    case taBUY:
        profit = (KFloat) OutExec.volume * (taOS->OrderParms.valueoftick *
                                            (OutExec.price - taOS->InExec[in].price) /
                                            taOS->OrderParms.ticksize - taOS->OrderParms.slippage) -
                 2 * taOS->OrderParms.commission;
        taOS->Long.trades++;
        if (profit > 0)
        {
            taOS->Long.win++;
            taOS->Long.winstreak++;
            taOS->Long.grossprofit += profit;
            if (profit > taOS->Long.largestwin) taOS->Long.largestwin = profit;
            taOS->Long.lossstreak = 0;
            taOS->Long.maxconwin = max(taOS->Long.maxconwin, taOS->Long.winstreak);
            taOS->Long.winbars += OutExec.barnum - taOS->InExec[in].barnum + 1;
        }
        if (profit < 0)
        {
            taOS->Long.loss++;
            taOS->Long.lossstreak++;
            taOS->Long.grossloss += profit;
            if (profit < taOS->Long.largestloss) taOS->Long.largestloss = profit;
            taOS->Long.winstreak = 0;
            taOS->Long.maxconloss = max(taOS->Long.maxconloss, taOS->Long.lossstreak);
            taOS->Long.lossbars += OutExec.barnum - taOS->InExec[in].barnum + 1;
        }
        taOS->Long.netprofit = taOS->Long.grossprofit + taOS->Long.grossloss;
        if (taOS->Long.grossloss)
            taOS->Long.profitfactor = fabs(taOS->Long.grossprofit / taOS->Long.grossloss);
        longmaxdrawdown= OutExec.volume * taOS->OrderParms.valueoftick *
                         (low - high) / taOS->OrderParms.ticksize;
        taOS->Long.maxdrawdown = min(taOS->Long.maxdrawdown, longmaxdrawdown);
        if (taOS->OrderParms.margin)
        {
            if (taOS->OrderParms.margin == 100)
                taOS->Long.acctsizereqd = fabs(taOS->Long.maxdrawdown);
            else
                taOS->Long.acctsizereqd = fabs(taOS->Long.maxdrawdown / (taOS->OrderParms.margin / 100));
        }
        if (taOS->Long.acctsizereqd)
            taOS->Long.rtnonacct = 100 * taOS->Long.netprofit / taOS->Long.acctsizereqd;

        break;
    }

    if (taOS->Long.win)
    {
        taOS->Long.avgwin = taOS->Long.grossprofit / taOS->Long.win;
        taOS->Long.avgwinstreak = taOS->Long.winbars / taOS->Long.win;
    }
    if (taOS->Long.loss)
    {
        taOS->Long.avgloss = taOS->Long.grossloss / taOS->Long.loss;
        taOS->Long.avglosstreak = taOS->Long.lossbars / taOS->Long.loss;
    }
    if (taOS->Long.trades)
    {
        taOS->Long.pctprofitable = ((KFloat) taOS->Long.win / (KFloat) taOS->Long.trades) * 100;
        taOS->Long.avgtrade = taOS->Long.netprofit / taOS->Long.trades;
    }
    if (taOS->Long.avgloss)
        taOS->Long.avgwintoavgloss = fabs(taOS->Long.avgwin / taOS->Long.avgloss);
    if (taOS->Short.win)
    {
        taOS->Short.avgwin = taOS->Short.grossprofit / taOS->Short.win;
        taOS->Short.avgwinstreak = taOS->Short.winbars / taOS->Short.win;
    }
    if (taOS->Short.loss)
    {
        taOS->Short.avgloss = taOS->Short.grossloss / taOS->Short.loss;
        taOS->Short.avglosstreak = taOS->Short.lossbars / taOS->Short.loss;
    }
    if (taOS->Short.trades)
    {
        taOS->Short.pctprofitable = ((KFloat) taOS->Short.win / (KFloat) taOS->Short.trades) * 100;
        taOS->Short.avgtrade = taOS->Short.netprofit / taOS->Short.trades;
    }
    if (taOS->Short.avgloss)
        taOS->Short.avgwintoavgloss = fabs(taOS->Short.avgwin / taOS->Short.avgloss);
    if (taOS->Long.win + taOS->Short.win)
        taOS->Net.avgwinstreak = (taOS->Long.winbars + taOS->Short.winbars) / (taOS->Long.win + taOS->Short.win);
    if (taOS->Long.loss + taOS->Short.loss)
        taOS->Net.avglosstreak = (taOS->Long.lossbars + taOS->Short.lossbars) / (taOS->Long.loss + taOS->Short.loss);

    taOS->Net.netprofit = taOS->Long.netprofit + taOS->Short.netprofit;
    taOS->Net.grossprofit = taOS->Long.grossprofit + taOS->Short.grossprofit;
    taOS->Net.trades = taOS->Long.trades + taOS->Short.trades;
    taOS->Net.win = taOS->Long.win + taOS->Short.win;
    taOS->Net.largestwin = max(taOS->Long.largestwin, taOS->Short.largestwin);
    if (taOS->Net.win) taOS->Net.avgwin = taOS->Net.grossprofit / taOS->Net.win;
    taOS->Net.maxconwin = max(taOS->Long.maxconwin, taOS->Short.maxconwin);

    taOS->Net.maxdrawdown = min(taOS->Long.maxdrawdown, taOS->Short.maxdrawdown);
    taOS->Net.profitfactor = taOS->Long.profitfactor + taOS->Short.profitfactor;
    if (taOS->OrderParms.margin)
        taOS->Net.acctsizereqd = fabs(min(taOS->Long.maxdrawdown, taOS->Short.maxdrawdown) /
                                      (taOS->OrderParms.margin / 100));
    taOS->Net.grossloss = taOS->Long.grossloss + taOS->Short.grossloss;
    if (taOS->Net.trades)
        taOS->Net.pctprofitable = ((KFloat) taOS->Net.win / (KFloat) taOS->Net.trades) * 100;

    taOS->Net.loss = taOS->Long.loss + taOS->Short.loss;
    taOS->Net.largestloss = min(taOS->Long.largestloss, taOS->Short.largestloss);
    if (taOS->Net.loss) taOS->Net.avgloss = taOS->Net.grossloss / taOS->Net.loss;
    if (taOS->Net.avgloss)
        taOS->Net.avgwintoavgloss = fabs(taOS->Net.avgwin / taOS->Net.avgloss);
    if (taOS->Net.trades)
        taOS->Net.avgtrade = taOS->Net.netprofit / taOS->Net.trades;
    taOS->Net.maxconloss = max(taOS->Long.maxconloss, taOS->Short.maxconloss);

    taOS->Net.maxcontractsheld = max(taOS->Long.maxcontractsheld, taOS->Short.maxcontractsheld);
    if (taOS->Net.acctsizereqd)
        taOS->Net.rtnonacct = 100 * taOS->Net.netprofit / taOS->Net.acctsizereqd;

    f1 = 0;
    f2 = 0;
    if (taOS->Net.win)
        f1 = (((taOS->Net.win - pow(taOS->Net.win, .5)) / taOS->Net.trades) *
              taOS->Net.grossprofit / taOS->Net.win);
    if (taOS->Net.loss)
        f2 = (((taOS->Net.loss + pow(taOS->Net.loss, .5)) / taOS->Net.trades) *
              -taOS->Net.grossloss / taOS->Net.loss);
    if (f2) taOS->Net.prr = f1 / f2;
    else taOS->Net.prr = f1;

    f1 = 0;
    f2 = 0;
    if (taOS->Short.win)
        f1 = (((taOS->Short.win - pow(taOS->Short.win, .5)) / taOS->Short.trades) *
              taOS->Short.grossprofit / taOS->Short.win);
    if (taOS->Short.loss)
        f2 = (((taOS->Short.loss + pow(taOS->Short.loss, .5)) / taOS->Short.trades) *
              -taOS->Short.grossloss / taOS->Short.loss);
    if (f2) taOS->Short.prr = f1 / f2;
    else taOS->Short.prr = f1;

    f1 = 0;
    f2 = 0;
    if (taOS->Long.win)
        f1 = (((taOS->Long.win - pow(taOS->Long.win, .5)) / taOS->Long.trades) *
              taOS->Long.grossprofit / taOS->Long.win);
    if (taOS->Long.loss)
        f2 = (((taOS->Long.loss + pow(taOS->Long.loss, .5)) / taOS->Long.trades) *
              -taOS->Long.grossloss / taOS->Long.loss);
    if (f2) taOS->Long.prr = f1 / f2;
    else taOS->Long.prr = f1;

    return (profit);
}


int      taUpdateCurrent(struct taOrderSystem *taOS)
{
    int      i1 = 0;
    long     longvol = 0, shortvol = 0, totalvolume = 0;
    KFloat   avgentryprice = 0;
    KFloat   totpricebuy = 0, totpricesell = 0;

    taOS->Long.openpospl = 0;
    taOS->Short.openpospl = 0;
    taOS->Net.openpospl = 0;

    if (!taOS->InExecCnt) return (0);

    taInitCurrentParms(taOS);
    for (i1 = 0; i1 < taOS->InExecCnt; i1++)
    {
        if (taOS->InExec[i1].remvolume)
        {
            switch (taOS->InExec[i1].buysell)
            {
            case taBUY:
                taOS->Current.longvol += taOS->InExec[i1].remvolume;
                totpricebuy += taOS->InExec[i1].remvolume * taOS->InExec[i1].price;
                if (!taOS->Current.longduration)
                    taOS->Current.longduration = taOS->Current.barnum - taOS->InExec[i1].barnum + 1;
                taOS->Long.openpospl += (KFloat) taOS->InExec[i1].remvolume *
                                        (taOS->OrderParms.valueoftick *
                                        (taArrayItem(taOS->bars.cl, taOS->Current.barnum) - taOS->InExec[i1].price)
                                        / taOS->OrderParms.ticksize);
                break;

            case taSELL:
                taOS->Current.shortvol += taOS->InExec[i1].remvolume;
                totpricesell += taOS->InExec[i1].remvolume * taOS->InExec[i1].price;
                if (!taOS->Current.shortduration)
                    taOS->Current.shortduration = taOS->Current.barnum - taOS->InExec[i1].barnum + 1;
                taOS->Short.openpospl += (KFloat) taOS->InExec[i1].remvolume * (taOS->OrderParms.valueoftick *
                                         (taOS->InExec[i1].price - taArrayItem(taOS->bars.cl, taOS->Current.barnum))
                                         / taOS->OrderParms.ticksize);
                break;
            }
        }
    }
    taOS->Net.openpospl = taOS->Long.openpospl + taOS->Short.openpospl;
    if (taOS->Current.longvol == taOS->Current.shortvol)
    {
        taOS->Current.position = 0;
        taOS->Current.totalvolume = 0;
    }
    if (taOS->Current.longvol > taOS->Current.shortvol)
    {
        taOS->Current.position = taLONG;
        taOS->Current.avgentryprice = totpricebuy / taOS->Current.longvol;
        taOS->Current.duration = taOS->Current.longduration;
    }
    else if (taOS->Current.shortvol)
    {
        taOS->Current.position = taSHORT;
        taOS->Current.avgentryprice = totpricesell / taOS->Current.shortvol;
        taOS->Current.duration = taOS->Current.shortduration;
    }
    else
    {
        taOS->Current.position = 0;
        taOS->Current.avgentryprice = 0;
    }
    taOS->Current.totalvolume = labs(taOS->Current.longvol - taOS->Current.shortvol);
    taOS->Short.maxcontractsheld = max(taOS->Short.maxcontractsheld, (int) taOS->Current.shortvol);
    taOS->Long.maxcontractsheld = max(taOS->Long.maxcontractsheld, (int) taOS->Current.longvol);
    return (0);
}


int taSortOrders(const void *elem1v, const void *elem2v)
{
    struct taOrder *elem1;
    struct taOrder *elem2;

    elem1 = (struct taOrder *)elem1v;
    elem2 = (struct taOrder *)elem2v;
    if (elem1->type > elem2->type) return (-1);
    if (elem1->type < elem2->type) return (1);
    if (elem1->buysell < elem2->buysell) return (-1);
    if (elem1->buysell > elem2->buysell) return (1);
    if (elem1->reqprice == elem2->reqprice) return (0);
    if (elem1->buysell == taBUY)
    {
        if (elem1->reqprice < elem2->reqprice) return (-1);
        if (elem1->reqprice > elem2->reqprice) return (1);
    }
    else
    {
        if (elem1->reqprice > elem2->reqprice) return (-1);
        if (elem1->reqprice < elem2->reqprice) return (1);
    }
    return (0);
}


