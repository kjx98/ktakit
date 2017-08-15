/* Technical Analysis Programmer's Toolkit - Copyright (C) 1997 KTA */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include "ktakit.h"
#include "tacsi.h"

int     taCSIfindfile(char *path, char *name, struct taCSImrec *mrec1);
void    taCSIsymbol(struct taCSImrec mrec, char *symbol);
KFloat  taCSIexpand(float number, unsigned int ex1, unsigned int ex2);
KFloat  taCSIcvt(KFloat number, int  cfac);
unsigned int    taCSIdatetojulian(float date);


int taCSIfindfile(char *path, char *name, struct taCSImrec *mrec1)
{
int     err = 0, i1;
char    buffer[80], symbol[16], filename[16];
FILE    *fp1;
struct taCSImrec mrec;

    strcpy(buffer, path);
    if (path[0] != '\0' && path[strlen(path) - 1] != ':' && path[strlen(path) - 1] != '\\')
        strcat(buffer, "\\");
    strcat(buffer, "qmaster");
    if ((fp1 = fopen(buffer, "rb")) == NULL) return (taErrFileOpen);

    for (i1 = 0; i1 < 120; i1++)
    {
        if (fread((char *) &mrec, sizeof(struct taCSImrec), 1, fp1) == 0)
        {
            err = taErrFileRead;
            break;
        }
        if (mrec.dflag == '0')
        {
            taCSIsymbol(mrec, symbol);
            strncpy(buffer, mrec.name, 20);
            buffer[20] = '\0';
            taRtrim(buffer);
            sprintf(filename, "F%03.3i.DTA", i1 + 1);
            if (!stricmp(name, symbol) || !stricmp(name, buffer) || !stricmp(name, filename))
            {
                memcpy(mrec1, &mrec, sizeof(struct taCSImrec));
                break;
            }
        }
        else
        {
            if (mrec.id[0] == '9')   /* end of block */
            {
                err = taErrFileNotFound;
                break;
            }
        }
    }
    if (i1 == 120) err = taErrFileNotFound;
    fclose(fp1);
    return (err)?err:(i1 + 1);
}


void    taCSIsymbol(struct taCSImrec mrec, char *symbol)
{
    if (mrec.csflag == 'S')
    {
        sprintf(symbol, "%6.6s", mrec.ssym);
        taRtrim(symbol);
        return;
    }

    if (mrec.csym[0] == mrec.ssym[0] && mrec.csym[1] == mrec.ssym[1] && mrec.ssym[2] != ' ')
    {
        sprintf(symbol, "%6.6s", mrec.ssym);
    }
    else
    {
        symbol[0] = mrec.csym[0];
        symbol[1] = mrec.csym[1];
        symbol[2] = '\0';
    }
    taRtrim(symbol);
    if (mrec.dm[1] == '0')
    {
        mrec.dm[1] = mrec.dm[0];
        mrec.dm[0] = '0';
    }
    sprintf(symbol, "%s%2.2s%2.2s", symbol, mrec.dm, mrec.dy);
    return;
}


KFloat   taCSIexpand(float number, unsigned int ex1, unsigned int ex2)
{
    return (number + (KFloat)(ex1 + ex2 * 4) * 65536);
}


KFloat   taCSIcvt(KFloat number, int cfac)
{
KFloat  div, rem;
double  num;

    if (cfac == 0) return (number);
    if (cfac > 0 && cfac < 100) return (number / pow(10, cfac));
    cfac = abs(cfac);
    if (cfac == 1) div = 10;
    else if (cfac <= 4) div = 100;
    else div = 1000;
    number = number / div;
    rem = modf(number, &num);
    if (rem == 0) return (num);
    number = num + (div * rem) / (8 * pow(2, cfac - 1));
    return (number);
}


unsigned int taCSIdatetojulian(float date)
{
int     year, month, day;
    year = (int) (date / 10000);
    month = (int) ((date - year * 10000) / 100);
    day = (int) ((date - year * 10000 - month * 100));
    return (taYMDToJulian(year + 1900,  month, day));
}


int DLL_EXPORT taCSIlist(char *path, char *outfile, int append)
{
int     i1, err = 0, line = 0, page = 0;
char    buffer[80], symbol[8];
FILE    *fp1, *out;
struct taCSImrec mrec;

#ifndef _WINDOWS
    if (!stricmp(outfile, "STDOUT"))
    {
        out = stdout;
        if (append) page = 1;
    }
    else
#endif
    {
        if (append) out = fopen(outfile, "at");
        else out = fopen(outfile, "wt");
        if (out == NULL) return (taErrFileOpen);
    }
    strcpy(buffer, path);
    if (path[0] != '\0' && path[strlen(path) - 1] != ':' && path[strlen(path) - 1] != '\\')
        strcat(buffer, "\\");
    strcat(buffer, "qmaster");
    if ((fp1 = fopen(buffer, "rb")) == NULL)
    {
#ifndef _WINDOWS
        if (stricmp(outfile, "STDOUT"))
#endif
            fclose(out);
        return (taErrFileOpen);
    }

    fprintf(out, "NUM SYMBOL  T NAME\n");
    fprintf(out, "--- ------  - --------------------\n");
    line = 2;
    for (i1 = 0; i1 < 120; i1++)
    {
        if (fread((char *) &mrec, sizeof(struct taCSImrec), 1, fp1) == 0)
        {
            err = taErrFileRead;
            break;
        }
        if (mrec.dflag == '0')
        {
            line++;
#ifndef _WINDOWS
            if (page && line == 25)
            {
                printf("Press any key to continue . . .");
                getchar();
                printf("\r                               \r");
                line = 1;
            }
#endif
            taCSIsymbol(mrec, symbol);
            fprintf(out, "%3i %-7s %c %20.20s\n", i1 + 1, symbol, mrec.type, mrec.name);
        }
        else
        {
            if (mrec.id[0] == '9')
            break;
        }
    }
    fclose(fp1);

#ifndef _WINDOWS
    if (stricmp(outfile, "STDOUT"))
#endif
    fclose(out);
    return (err);
}


long DLL_EXPORT taCSIfindrec(char *path, char *name, unsigned int date, unsigned int *actualdate)
{
int     err = 0, fnum;
int     year, month, day;
float   dt = 0;
int     recno = -1, guess = 0, lastguess = 0;
int     maxent = 0, low = 0, high = 0;
char    buffer[80];
FILE    *fp1;
struct taCSImrec mrec;
struct taCSIrec rec;
struct taCSIhdr hdr;

    if ((fnum = taCSIfindfile(path, name, &mrec)) < 0) return (fnum);
    if (path[0] != '\0' && path[strlen(path) - 1] != ':' && path[strlen(path) - 1] != '\\')
        sprintf(buffer, "%s\\F%03.3i.DTA", path, fnum);
    else sprintf(buffer, "%sF%03.3i.DTA", path, fnum);
    if ((fp1 = fopen(buffer, "rb")) == NULL) return (taErrFileOpen);
    if (fread((char *) &hdr, sizeof(struct taCSIhdr), 1, fp1) <= 0)
    {
        fclose(fp1);
        return (taErrFileRead);
    }
    taMStoIEEE(&hdr.ferp, &hdr.ferp);
    taMStoIEEE(&hdr.mdp, &hdr.mdp);
    taMStoIEEE(&hdr.hh, &hdr.hh);
    taMStoIEEE(&hdr.ll, &hdr.ll);
    taMStoIEEE(&hdr.fpdof, &hdr.fpdof);
    taMStoIEEE(&hdr.lpdof, &hdr.lpdof);

    if (date == 0) dt = 0;
    else
    {
        taJulianToYMD(date, &year, &month, &day);
        dt = (float)(year - 1900) * 10000 + month * 100 + day;
    }
    if (dt > hdr.lpdof)
    {
        fclose(fp1);
        if (actualdate) *actualdate = taCSIdatetojulian(hdr.lpdof);
        return (taErrRecordNotFound);
    }
    maxent = (long) hdr.mdp - 1;
    high = maxent;
    while (recno < 0 && !err)
    {
        guess = (low + high + 1) / 2;
        if (guess == lastguess)
        {
            if (low == 0) guess = 0;
            else
            {
                recno = guess;
                break;
            }
        }
        if (fseek(fp1, guess * sizeof_taCSIrec + sizeof(struct taCSIhdr), SEEK_SET))
        {
            err = taErrFileRead;
            break;
        }
        if (!fread(&rec, sizeof_taCSIrec, 1, fp1))
        {
            err = taErrFileRead;
            break;
        }
        taMStoIEEE(&rec.date, &rec.date);
        if (rec.date == dt || guess == 0)
        {
            recno = guess;
            break;
        }
        if (rec.date > dt) high = guess;
    if (rec.date < dt) low = guess;
    lastguess = guess;
    }

    fclose(fp1);
    if (actualdate) *actualdate = taCSIdatetojulian(rec.date);
    if (err) return (err);
    return (recno);
}


int DLL_EXPORT taCSIread(taBars *b1, char *path, char *name, long start, int cnt, int allocate)
{
int     i1 = 0, err = 0, cf, fnum;
int     recnum;
char    buffer[80];
FILE    *fp1;
struct taCSImrec mrec;
struct taCSIrec rec;
struct taCSIhdr hdr;

    if ((fnum = taCSIfindfile(path, name, &mrec)) < 0) return (fnum);
    if (path[0] != '\0' && path[strlen(path) - 1] != ':' && path[strlen(path) - 1] != '\\')
        sprintf(buffer, "%s\\F%03.3i.DTA", path, fnum);
    else sprintf(buffer, "%sF%03.3i.DTA", path, fnum);
    if ((fp1 = fopen(buffer, "rb")) == NULL) return (taErrFileOpen);
    if (fread((char *) &hdr, sizeof(struct taCSIhdr), 1, fp1) <= 0)
    {
        fclose(fp1);
        return (taErrFileRead);
    }
    taMStoIEEE(&hdr.ferp, &hdr.ferp);
    taMStoIEEE(&hdr.mdp, &hdr.mdp);
    taMStoIEEE(&hdr.hh, &hdr.hh);
    taMStoIEEE(&hdr.ll, &hdr.ll);
    taMStoIEEE(&hdr.fpdof, &hdr.fpdof);
    taMStoIEEE(&hdr.lpdof, &hdr.lpdof);

    if (cnt < 0)
    {
        if (start > 0) cnt = min((short ) hdr.mdp, 16000) - start;
        else cnt = min((short ) hdr.mdp, 16000);
    }

    if (allocate)
    {
        if ((err = taAllocBars(b1, cnt)))
        {
            fclose(fp1);
            return (err);
        }
    }
    buffer[0] = mrec.cf[0];
    buffer[1] = mrec.cf[1];
    buffer[2] = '\0';
    cf = atoi(buffer);

    b1->reccnt = (long) hdr.mdp - 1;
#ifdef OMMIT
 strcpy(b1->path, path);
 strcpy(b1->name, name);
 strncpy(b1->description, mrec.name, 20);
 b1->description[20]  = '\0';
 taRtrim(b1->description);
 taCSIsymbol(mrec, b1->symbol);
 b1->cusip[0] = '\0';
 b1->begindate = taCSIdatetojulian(hdr.fpdof);
 b1->enddate = taCSIdatetojulian(hdr.lpdof);
#endif
    if (mrec.type == 'D') b1->frequency = taDAILY;
    else if (mrec.type == 'W') b1->frequency = taWEEKLY;
    else if (mrec.type == 'M') b1->frequency = taMONTHLY;
    else b1->frequency = taUNKNOWN;
    if (mrec.csflag == 'C') b1->datatype = taCOMMODITY;
    else if (mrec.csflag == 'S') b1->datatype = taSTOCK;
    else b1->datatype = taUNKNOWN;
    if (mrec.oflag == 'P') b1->optiontype = taPUT;
    else if (mrec.oflag == 'C') b1->optiontype = taCALL;
    else b1->optiontype = taUNKNOWN;
 #ifdef    OMMIT
 sprintf(buffer, "%2.2s%2.2s01", mrec.dy, mrec.dm);
 b1->deliverydate = taDateTxtToJulian(buffer, DTFORMAT_YYMMDD);
 b1->strikeprice = atof(mrec.sprice);
 b1->type = taCSI;
 #endif
    if (cnt == 0)
    {
        fclose(fp1);
        return (0);
    }
    cnt = min(cnt, b1->datasize);
    if (start < 0) start = max(0, b1->reccnt - cnt);

    if (fseek(fp1, start * sizeof_taCSIrec + sizeof(struct taCSIhdr), SEEK_SET))
    {
        fclose(fp1);
        return (taErrFileSeek);
    }
    i1 = 0;
    recnum = start;
    while (i1 < cnt && recnum < (short)hdr.mdp && fread((char *)&rec, sizeof_taCSIrec, 1, fp1) > 0)
    {
        recnum++;
        taMStoIEEE(&rec.date, &rec.date);
        if (rec.dow == 9 || (long)rec.date == 0) continue;
        taArrayItem(b1->dt, i1) = taCSIdatetojulian(rec.date);
        taArrayItem(b1->ti, i1) = 0;
        taArrayItem(b1->op, i1) = taCSIcvt(taCSIexpand(rec.open, rec.exop, rec.exop2), cf);
        taArrayItem(b1->hi, i1) = taCSIcvt(taCSIexpand(rec.high, rec.exhi, rec.exhi2), cf);
        taArrayItem(b1->lo, i1) = taCSIcvt(taCSIexpand(rec.low, rec.exlo, rec.exlo2), cf);
        taArrayItem(b1->cl, i1) = taCSIcvt(taCSIexpand(rec.close, rec.excl, rec.excl2), cf);
        taArrayItem(b1->vol, i1) = taCSIexpand(rec.tvol, rec.tvol1, 0);
        taArrayItem(b1->oi, i1) = taCSIexpand(rec.toi, rec.toi1, 0);
        i1++;
    }
    fclose(fp1);
    taReSizeBars(b1, i1);
    return (err);
}

