/* Technical Analysis Programmer's Toolkit - Copyright (C) 1997 KTA */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include "ktakit.h"


int DLL_EXPORT taFILEtype(char *type)
{
    int     i1;
    static char *typenames[6] = {"CSI","CT","DJ","FS","TC","TT"};
    static int typenums[7] = {taCSI,taCT,taDJ,taFS,taTC,taTT,taTXT};

    for (i1 = 0; i1 < 6; i1++)
        if (!stricmp(type, typenames[i1])) break;
    return (typenums[i1]);
}


long DLL_EXPORT taFILEfindrec(char *type, char *path, char *name,
                              unsigned int date, unsigned int time,
                              unsigned int *actualdate, unsigned int *actualtime)
{
    int      err;
    struct taTXTinfo txtinfo;

    *actualdate = 0;
    *actualtime = 0;
    switch (taFILEtype(type))
    {
    case taTXT:
        if ((err = taTXTreadinfo(path, type, &txtinfo)))
            return (err);
        return (taTXTfindrec(path, name, &txtinfo, date, time, actualdate, actualtime));

    case taCSI:
        return (taCSIfindrec(path, name, date, actualdate));

    case taCT:
        return (taCTfindrec(path, name, date, time, actualdate, actualtime));

    case taDJ:
        return (taDJfindrec(path, name, date, actualdate));

    case taFS:
        return (taFSfindrec(path, name, date, time, actualdate, actualtime));

    case taTC:
        return (taTCfindrec(path, name, date, actualdate));

    case taTT:
        return (taTTfindrec(path, name, date, actualdate));
    }
    return (0);
}


int DLL_EXPORT taFILElist(char *type, char *path, char *outfile, int append)
{
    switch (taFILEtype(type))
    {
    case taCSI:
        return (taCSIlist(path, outfile, append));

    case taCT:
        return (taCTlist(path, outfile, append));

    case taDJ:
        return (taDJlist(path, outfile, append));

    case taTC:
        return (taTClist(path, outfile, append));
    }
    return (0);
}


int DLL_EXPORT taFILEprintinfo(taBars *b1, char *outfile, int append)
{
    FILE    *out;
    char     buffer[12];
    static char *type[8] = {"Unknown","CSI","CompuTrac","Dow Jones","FutureSource","TeleChart","TechTools","Text"};
    static char *frequency[7] = {"Unknown","Intraday","Daily","Weekly","Monthly","Quarterly","Yearly"};
    static char *datatype[7] = {"Unknown","Bond","Commodity","Mutual Fund","Option","Stock","Index"};
    static char *optiontype[3] = {" ","Call","Put"};

#ifndef _WINDOWS
    if (!stricmp(outfile, "STDOUT")) out = stdout;
    else
#endif
    {
        if (append) out = fopen(outfile, "at");
        else out = fopen(outfile, "wt");
        if (out == NULL) return (taErrFileOpen);
    }
    fprintf(out, "barsize      %i\n", b1->size);
    fprintf(out, "datasize     %i\n", b1->datasize);
    fprintf(out, "reccnt       %li\n", b1->reccnt);
    fprintf(out, "name         %s\n", b1->name);
#ifdef    OMMIT
    fprintf(out, "path         %s\n", b1->path);
    fprintf(out, "description  %s\n", b1->description);
    fprintf(out, "symbol       %s\n", b1->symbol);
    fprintf(out, "cusip        %s\n", b1->cusip);
    taJulianToDateTxt(b1->begindate, DTFORMAT_MM_DD_YY, "/", buffer);
    fprintf(out, "begindate    %s\n", buffer);
    taJulianToDateTxt(b1->enddate, DTFORMAT_MM_DD_YY, "/", buffer);
    fprintf(out, "enddate      %s\n", buffer);
    taJulianToDateTxt(b1->deliverydate, DTFORMAT_MM_DD_YY, "/", buffer);
    fprintf(out, "deliverydate %s\n", buffer);
    fprintf(out, "type         %s\n", type[b1->type]);
#endif
    fprintf(out, "frequency    %s\n", frequency[b1->frequency]);
    fprintf(out, "datatype     %s\n", datatype[b1->datatype]);
    fprintf(out, "optiontype   %s\n", optiontype[b1->optiontype]);
    fprintf(out, "strikeprice  %8li.%02li\n", (long)b1->strikeprice, taDecimals(b1->strikeprice, 2));

#ifndef _WINDOWS
    if (stricmp(outfile, "STDOUT"))
#endif
        fclose(out);
    return (0);
}


int DLL_EXPORT taFILEread(char *type, taBars *b1, char *path, char *name,
                          long start, int cnt, int allocate)
{
    int      err;
    struct taTXTinfo txtinfo;

    switch (taFILEtype(type))
    {
    case taTXT:
        if ((err = taTXTreadinfo(path, type, &txtinfo)))
            return (err);
        return (taTXTread(b1, path, name, &txtinfo, start, cnt, allocate));

    case taCSI:
        return (taCSIread(b1, path, name, start, cnt, allocate));

    case taCT:
        return (taCTread(b1, path, name, start, cnt, allocate));

    case taDJ:
        return (taDJread(b1, path, name, start, cnt, allocate));

    case taFS:
        return (taFSread(b1, path, name, start, cnt, allocate));

    case taTC:
        return (taTCread(b1, path, name, start, cnt, allocate));

    case taTT:
        return (taTTread(b1, path, name, start, cnt, allocate));
    }
    return (-1);
}

