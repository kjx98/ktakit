/* Technical Analysis Programmer's Toolkit - Copyright (C) 1997 KTA */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include "ktakit.h"
#include "tafs.h"

int taFSfracrec(struct taFSfracs *fracs, int record, char *path);
KFloat   taFSfixprice(struct taFSfracs fracs, unsigned long price);


long DLL_EXPORT taFSfindrec(char *path, char *name, unsigned int date, unsigned int time,
                            unsigned int *actualdate, unsigned int *actualtime)
{
    int    err = 0;
    long   recno = -1, guess = 0, lastguess = 0;
    long   maxent = 0, low = 0, high = 0;
    unsigned short  dt = 0, ti = 0;
    char     buffer[81];
    FILE    *fp1;
    struct taFSinfo info;

    strcpy(buffer, path);
    if (buffer[0] != '\0' && buffer[strlen(buffer) - 1] != ':' && buffer[strlen(buffer) - 1] != '\\')
        strcat(buffer, "\\");
    strcat(buffer, name);
    if ((fp1 = fopen(buffer, "rb")) == NULL)
        return (taErrFileOpen);
    fread((char *) &info, sizeof(struct taFSinfo), 1, fp1);
    if (info.version)
    {
        fclose(fp1);
        return (taErrInvalidParm);
    }
    fseek(fp1, 0L, SEEK_END);
    maxent = (ftell(fp1) - info.dataoffset) / info.recordlen;
    high = maxent;
    while (recno < 0 && !err)
    {
        guess = (low + high + 1) / 2;
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
        if (fseek(fp1, guess * info.recordlen + info.dataoffset, SEEK_SET))
        {
            err = taErrFileRead;
            break;
        }
        fread((char *) &dt, sizeof(short), 1, fp1);
        if (info.filetype == 'V')
            fread((char *) &ti, sizeof(short), 1, fp1);
        if ((dt == date && ti == time) || guess == 0)
        {
            recno = guess;
            break;
        }
        if (dt > date || (dt == date && ti > time))
            high = guess;
        if (dt < date || (dt == date && ti < time))
            low = guess;
        lastguess = guess;
    }
    fclose(fp1);
    if (dt < date || (dt == date && ti < time))
        err = taErrRecordNotFound;
    if (actualdate)
        *actualdate = dt;
    if (actualtime)
        *actualtime = ti;
    if (err)
        return (err);
    return (recno);
}


int DLL_EXPORT taFSread(taBars *b1, char *path, char *name, long start, int cnt, int allocate)
{
    int    i1, err, recno, done = 0;
    long   l1, reccnt;
    char   buffer[81];
    FILE    *fp1;
    struct taFSinfo info, voihdr;
    struct taFSdaily16 daily16;
    struct taFSdaily32 daily32;
    struct taFSvar16 var16;
    struct taFSvar32 var32;
    struct taFSvoi16 voi16;
    struct taFSvoi32 voi32;
    struct taFSfracs fracs;

    strcpy(buffer, path);
    if (buffer[0] != '\0' && buffer[strlen(buffer) - 1] != ':' && buffer[strlen(buffer) - 1] != '\\')
        strcat(buffer, "\\");
    strcat(buffer, name);

    if ((fp1 = fopen(buffer, "rb")) == NULL)
        return (taErrFileOpen);
    fread((char *) &info, sizeof(struct taFSinfo), 1, fp1);
    if (info.version)
    {
        fclose(fp1);
        return (taErrInvalidParm);
    }

    if (info.filetype != 'V' && info.filetype != 'D' && info.filetype != 'P')
    {
        fclose(fp1);
        return (taErrInvalidParm);
    }

    fseek(fp1, 0L, SEEK_END);
    l1 = ftell(fp1);
    reccnt = (l1 - info.dataoffset) / info.recordlen;

    if (cnt < 0)
    {
        if (start > 0)
            cnt = min(reccnt, 16000) - start;
        else
            cnt = min(reccnt, 16000);
    }

    if (allocate)
    {
        if ((err = taAllocBars(b1, cnt)))
            return (err);
    }

    b1->reccnt = reccnt;
    strcpy(b1->name, name);
#ifdef OMMIT
    b1->type = taFS;
    strcpy(b1->path, path);
    strcpy(b1->description, info.desc);
    strcpy(b1->symbol, info.symbolname);
    b1->symbol[4] = '\0';
    b1->description[28] = '\0';
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
    if (start > reccnt)
    {
        fclose(fp1);
        return (taErrRecordNotFound);
    }
    recno = start;
    fseek(fp1, recno * info.recordlen + info.dataoffset, SEEK_SET);
    i1 = 0;
    cnt = min(cnt, b1->datasize);

    taFSfracrec(&fracs, info.fraction, path);

    if (info.filetype == 'V')
    {
        if (info.bit32values == 1)
        {
            while (i1 < cnt && recno <= reccnt && fread((char *) &var32, sizeof(struct taFSvar32), 1, fp1) > 0)
            {
                taArrayItem(b1->dt, i1) = var32.date;
                taArrayItem(b1->ti, i1) = var32.time;
                taArrayItem(b1->op, i1) = taFSfixprice(fracs, var32.open);
                taArrayItem(b1->hi, i1) = taFSfixprice(fracs, var32.high);
                taArrayItem(b1->lo, i1) = taFSfixprice(fracs, var32.low);
                taArrayItem(b1->cl, i1) = taFSfixprice(fracs, var32.close);
                taArrayItem(b1->vol, i1) = 0;
                taArrayItem(b1->oi, i1) = 0;
                i1++;
                recno++;
            }
        }
        else
        {
            while (i1 < cnt && recno <= reccnt && fread((char *) &var16, sizeof(struct taFSvar16), 1, fp1) > 0)
            {
                taArrayItem(b1->dt, i1) = var16.date;
                taArrayItem(b1->ti, i1) = var16.time;
                taArrayItem(b1->op, i1) = taFSfixprice(fracs, var16.open);
                taArrayItem(b1->hi, i1) = taFSfixprice(fracs, var16.high);
                taArrayItem(b1->lo, i1) = taFSfixprice(fracs, var16.low);
                taArrayItem(b1->cl, i1) = taFSfixprice(fracs, var16.close);
                taArrayItem(b1->vol, i1) = 0;
                taArrayItem(b1->oi, i1) = 0;
                i1++;
                recno++;
            }
        }
    }
    else  /* if (info.filetype == 'D') */
    {
        if (info.bit32values == 1)
        {
            while (i1 < cnt && recno <= reccnt && fread((char *) &daily32, sizeof(struct taFSdaily32), 1, fp1) > 0)
            {
                taArrayItem(b1->dt, i1) = daily32.date;
                taArrayItem(b1->ti, i1) = 0;
                taArrayItem(b1->op, i1) = taFSfixprice(fracs, daily32.open);
                taArrayItem(b1->hi, i1) = taFSfixprice(fracs, daily32.high);
                taArrayItem(b1->lo, i1) = taFSfixprice(fracs, daily32.low);
                taArrayItem(b1->cl, i1) = taFSfixprice(fracs, daily32.close);
                taArrayItem(b1->vol, i1) = 0;
                taArrayItem(b1->oi, i1) = 0;
                i1++;
                recno++;
            }
        }
        else
        {
            while (i1 < cnt && recno <= reccnt && fread((char *) &daily16, sizeof(struct taFSdaily16), 1, fp1) > 0)
            {
                taArrayItem(b1->dt, i1) = daily16.date;
                taArrayItem(b1->ti, i1) = 0;
                taArrayItem(b1->op, i1) = taFSfixprice(fracs, daily16.open);
                taArrayItem(b1->hi, i1) = taFSfixprice(fracs, daily16.high);
                taArrayItem(b1->lo, i1) = taFSfixprice(fracs, daily16.low);
                taArrayItem(b1->cl, i1) = taFSfixprice(fracs, daily16.close);
                taArrayItem(b1->vol, i1) = 0;
                taArrayItem(b1->oi, i1) = 0;
                i1++;
                recno++;
            }
        }
    }

    fclose(fp1);
    taReSizeBars(b1, i1);

    if (info.voifileexists)
    {
        buffer[strlen(buffer) - 2] = 'V';
        buffer[strlen(buffer) - 1] = 'I';
        if ((fp1 = fopen(buffer, "rb")) == NULL)
        {
            info.voifileexists = 0;
            return (0);
        }
        i1 = 0;
        voi32.date = 0;
        voi16.date = 0;
        fread((char *) &voihdr, sizeof(struct taFSinfo), 1, fp1);
        fseek(fp1, (long) (voihdr.dataoffset), SEEK_SET);
        if (voihdr.bit32values == 1)
        {
            while (!done)
            {
                if (taArrayItem(b1->dt, i1) < voi32.date)
                {
                    i1++;
                    if (i1 > b1->size)
                        done = 1;
                }
                else if (taArrayItem(b1->dt, i1) > voi32.date)
                {
                    if (fread((char *) &voi32, sizeof(struct taFSvoi32), 1, fp1) <= 0)
                        done = 1;
                }
                else
                {
                    taArrayItem(b1->vol, i1) = voi32.vol;
                    taArrayItem(b1->oi, i1) = voi32.oi;
                    i1++;
                }
            }
        }
        else /* 16bit */
        {
            while (!done)
            {
                if (taArrayItem(b1->dt, i1) < voi16.date)
                {
                    i1++;
                    if (i1 > cnt)
                        done = 1;
                }
                else if (taArrayItem(b1->dt, i1) > voi16.date)
                {
                    if (fread((char *) &voi16, sizeof(struct taFSvoi16), 1, fp1) <= 0)
                        done = 1;
                }
                else
                {
                    taArrayItem(b1->vol, i1) = voi16.vol;
                    taArrayItem(b1->oi, i1) = voi16.oi;
                    i1++;
                }
            }
        }
    }
    return (0);
}


int     taFSfracrec(struct taFSfracs *fracs, int record, char *path)
{
    char     buffer[81];
    FILE    *fp1;
    struct taFStablehdr tablehdr;

    fracs->xmitpower = 1;
    fracs->xmitdenom = 1;

    strcpy(buffer, path);
    if (buffer[0] != '\0' && buffer[strlen(buffer) - 1] != ':' && buffer[strlen(buffer) - 1] != '\\')
        strcat(buffer, "\\");
    strcat(buffer, "fsfracs.fil");

    if ((fp1 = fopen(buffer, "rb")) == NULL)
        return (-2);
    fread((char *) &tablehdr, sizeof(struct taFStablehdr), 1, fp1);
    if (tablehdr.version > 300)
        fseek(fp1, (long) ((1 + record) * tablehdr.recordlen), SEEK_SET);
    else
        fseek(fp1, (long) (4 + record * tablehdr.recordlen), SEEK_SET);
    fread(fracs, tablehdr.recordlen, 1, fp1);
    fclose(fp1);
    return (0);
}


KFloat   taFSfixprice(struct taFSfracs fracs, unsigned long price)
{
    KFloat   f1;
    ldiv_t   num;

    num = ldiv(price, (long)fracs.xmitpower);
    f1 = (KFloat)num.quot + (KFloat)num.rem / fracs.xmitdenom;
    return (f1);
}
