/* Technical Analysis Programmer's Toolkit - Copyright (C) 1997 KTA */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ktakit.h"

int      taChrCnt(char *string, char chr)
{
    int      i1 = 0, cnt = 0;

    for (i1 = 0; string[i1]; i1++)
        if (string[i1] == chr)
            cnt++;
    return (cnt);
}

int      taChrsCnt(char *string, char *chrs)
{
    int      i1 = 0, i2 = 0, cnt = 0;

    for (i2 = 0; chrs[i2] != '\0'; i2++)
        for (i1 = 0; string[i1] != '\0'; i1++)
            if (string[i1] == chrs[i2])
                cnt++;
    return (cnt);
}

void     taLtrim(char *string)
{
    int      i1;

    for (i1 = 0; string[i1] == ' ' && string[i1] != '\0'; i1++);
    memmove(string, string + i1, strlen(string) - i1 + 1);
}

void     taRtrim(char *string)
{
    int      i1;

    for (i1 = strlen(string) - 1; string[i1] == ' ' && i1 >= 0; i1--)
        string[i1] = '\0';
}

void     taTrim(char *string)
{
    taRtrim(string);
    taLtrim(string);
}

int      taStrCpyChr(char *to, char *from, char chr)
{
    int      i1;

    for (i1 = 0; from[i1] != chr && from[i1] != '\0'; i1++)
        to[i1] = from[i1];
    to[i1] = '\0';
    return (i1);
}

int      taStrNthChr(char *str, char chr, int n)
{
    int      i1, i2;

    for (i1 = 0, i2 = 0; i2 < n && str[i1] != '\0'; i1++)
        if (str[i1] == chr)
            i2++;
    return (i1 - 1);
}
