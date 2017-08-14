/* Technical Analysis Programmer's Toolkit - Copyright (C) 1996 KTA */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include "ktakit.h"
#include "tatt.h"


KFloat   taTT3bytetofloat(unsigned char *threebyter);
KFloat   taTTcvt(KFloat price, int conversion, int meat);


long DLL_EXPORT taTTfindrec(char *path, char *name, unsigned int date, unsigned int *actualdate)
{
    int    err = 0;
    long   recno = -1, guess = 0, lastguess = 0;
    long   maxent = 0, low = 0, high = 0;
    char   buffer[81];
    FILE   *fp1;
    struct taTTrec rec;

    strcpy(buffer, path);
    if (buffer[0] != '\0' && buffer[strlen(buffer) - 1] != ':' && buffer[strlen(buffer) - 1] != '\\')
        strcat(buffer, "\\");
    strcat(buffer, name);

    if ((fp1 = fopen(buffer, "rb")) == NULL)
        return (taErrFileOpen);
    fseek(fp1, 0, SEEK_END);
    maxent = ftell(fp1) / sizeof(struct taTTrec) - 1;
    fseek(fp1, 0, SEEK_SET);
    high = maxent;
    date += 22281;
    while (recno < 0 && !err)
    {
        guess = (low + high + 1) / 2;
        if (guess >= maxent)
            return(taErrRecordNotFound);
        if (guess == lastguess)
        {
            if (low == 0)
                guess = 0;
            else
            {
                recno = guess;
                break;
            }
        }
        if (fseek(fp1, (guess + 1) * sizeof(struct taTTrec), SEEK_SET))
        {
            err = taErrFileRead;
            break;
        }
        if (!fread(&rec, sizeof(struct taTTrec), 1, fp1))
        {
            err = taErrFileRead;
            break;
        }
        if (rec.dt == date || guess == 0)
        {
            recno = guess;
            break;
        }
        if (rec.dt > date)
            high = guess;
        if (rec.dt < date)
            low = guess;
        lastguess = guess;
    }
    fclose(fp1);
    if (actualdate)
        *actualdate = rec.dt - 21915;
    if (err)
        return (err);
    return (recno);
}


int DLL_EXPORT taTTread(taBars *b1, char *path, char *name, long start, int cnt, int allocate)
{
    int    i1 = 0, err = 0, conversion, meat = 0;
    long   reccnt;
    char   buffer[81];
    FILE   *fp1;
    struct taTTrec rec;

    strcpy(buffer, path);
    if (buffer[0] != '\0' && buffer[strlen(buffer) - 1] != ':' && buffer[strlen(buffer) - 1] != '\\')
        strcat(buffer, "\\");
    strcat(buffer, name);

    if ((fp1 = fopen(buffer, "rb")) == NULL) return (taErrFileOpen);

    if (fread((char *) &conversion, 2, 1, fp1) <= 0)
    {
        fclose(fp1);
        return (taErrFileRead);
    }
    fseek(fp1, 0, SEEK_END);
    reccnt = ftell(fp1) / sizeof(struct taTTrec) - 1;
    fseek(fp1, 0, SEEK_SET);

    if (cnt < 0)
    {
        if (start > 0) cnt = min((int) reccnt, 16000) - start;
        else cnt = min((int) reccnt, 16000);
    }

    if (allocate)
    {
        if ((err = taAllocBars(b1, cnt)))
        {
            fclose(fp1);
            return (err);
        }
    }

    b1->reccnt = reccnt;
    strcpy(b1->name, name);
#ifdef OMMIT
    b1->type = taTT;
    strcpy(b1->path, path);
    strcpy(b1->description, name);
    memset(b1->symbol, 0, 16);
    memset(b1->cusip, 0, 12);
    b1->begindate = 0;
    b1->enddate = 0;
    b1->deliverydate = 0;
#endif
    b1->frequency = 0;
    b1->datatype = 0;
    b1->optiontype = 0;
    b1->strikeprice = 0;

    if (cnt == 0)
    {
        fclose(fp1);
        return (0);
    }
    cnt = min(cnt, b1->datasize);
    if (start < 0)
        start = max(0, b1->reccnt - cnt);

    if (fseek(fp1, (start + 1) * sizeof(struct taTTrec), SEEK_SET))
    {
        fclose(fp1);
        return (taErrFileSeek);
    }
    if (!strncmp(name, "FC_", 3) || !strncmp(name, "LC_", 3) ||
            !strncmp(name, "LH_", 3) || !strncmp(name, "PB_", 3))
        meat = 1;
    for (i1 = 0; i1 < cnt && i1 + start < reccnt && fread((char *) &rec, 16, 1, fp1) > 0; i1++)
    {
        taArrayItem(b1->dt, i1) = rec.dt - 22281;
        taArrayItem(b1->ti, i1) = 0;
        taArrayItem(b1->op, i1) = taTTcvt(rec.op, conversion, meat);
        taArrayItem(b1->hi, i1) = taTTcvt(rec.hi, conversion, meat);
        taArrayItem(b1->lo, i1) = taTTcvt(rec.lo, conversion, meat);
        taArrayItem(b1->cl, i1) = taTTcvt(rec.cl, conversion, meat);
        taArrayItem(b1->vol, i1) = taTT3bytetofloat(rec.vol);
        taArrayItem(b1->oi, i1) = taTT3bytetofloat(rec.oi);
    }
    fclose(fp1);
    taReSizeBars(b1, i1);
    return (err);
}


KFloat   taTT3bytetofloat(unsigned char *threebyter)
{
    return ((long) threebyter[0] + ((long) threebyter[1] << 8) + ((long) threebyter[2] << 16));
}


KFloat   taTTcvt(KFloat price, int conversion, int meat)
{
    KFloat   newprice, whole, fraction;

    switch (conversion)
    {
    case 10:
        newprice = price * 10;
        break;

    case 5:
        newprice = price / 100000.0F;
        break;

    case 4:
        newprice = price / 10000;
        break;

    case 3:
        newprice = price / 1000;
        break;

    case 2:
        newprice = price / 100;
        if (meat)
            if ((int) (price) % 5 != 0) /* add to .02 and .07 */
                newprice += .005F;
        break;

    case 1:
        newprice = price / 10;
        break;

    case 0:
        newprice = price;
        break;

    case -1:
        whole = floor(price / 10);
        fraction = price - 10 * whole;
        newprice = whole + fraction / 2;
        break;

    case -2:
        whole = floor(price / 10);
        fraction = price - 10 * whole;
        newprice = whole + fraction / 4;
        break;

    case -3:
        whole = floor(price / 10);
        fraction = price - 10 * whole;
        newprice = whole + fraction / 8;
        break;

    case -4:
        whole = floor(price / 100);
        fraction = price - 100 * whole;
        newprice = whole + fraction / 16;
        break;

    case -5:
        whole = floor(price / 100);
        fraction = price - 100 * whole;
        newprice = whole + fraction / 32;
        break;

    case -6:
        whole = floor(price / 100);
        fraction = price - 100 * whole;
        newprice = whole + fraction / 64;
        break;

    case -7:
        whole = floor(price / 1000);
        fraction = price - 1000 * whole;
        newprice = whole + fraction / 128;
        break;

    case -8:
        whole = floor(price / 1000);
        fraction = price - 1000 * whole;
        newprice = whole + fraction / 256;
        break;

    case -9:
        whole = floor(price / 1000);
        fraction = price - 1000 * whole;
        newprice = whole + fraction / 512;
        break;

    case -11:
        newprice = price / 2.0F;
        break;

    case -12:
        newprice = price / 4.0F;
        break;

    case -13:
        newprice = price / 8.0F;
        break;

    case -14:
        newprice = price / 16.0F;
        break;

    case -15:
        newprice = price / 32.0;
        break;

    case -16:
        newprice = price / 64.0;
        break;

    case -17:
        newprice = price / 128.0;
        break;

    case -18:
        newprice = price / 256.0F;
        break;

    case -19:
        newprice = price / 512.0F;
        break;

    case -22:
        newprice = price * .05;
        break;

    case -24:
        newprice = price * .025;
        break;

    case -25:
        newprice = price * .02;
        break;

    case -32:
        newprice = price / 200.0;
        break;

    case -34:
        newprice = price / 400.0;
        break;

    case -35:
        newprice = price / 500.0;
        break;

    default:
        newprice = 0;
    }
    return (newprice);
}
