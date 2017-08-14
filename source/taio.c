/* Technical Analysis Programmer's Toolkit - Copyright (C) 1997 KTA */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include <stdarg.h>
#include "ktakit.h"


int DLL_EXPORT taPNFchart(taBars *pnfbars, KFloat box, char *outfile, int append)
{
    int    i1;
    KFloat    f1, min1, max1, tenth;
    FILE    *out;

#ifndef _WINDOWS
    if (!stricmp(outfile, "STDOUT"))
        out = stdout;
    else
#endif
    {
        if (append)
            out = fopen(outfile, "at");
        else
            out = fopen(outfile, "wt");
        if (out == NULL)
            return (taErrFileOpen);
    }
    tenth = box / 10;
    taArrayStats(&pnfbars->hi, NULL, &max1, NULL, NULL, 0, pnfbars->size, 1);
    taArrayStats(&pnfbars->lo, &min1, NULL, NULL, NULL, 0, pnfbars->size, 1);

    max1 = max1 + box / 10 - fmod(max1 + box / 10, box);
    min1 = min1 - fmod(min1, box) - tenth;

    for (f1 = max1; f1 >= min1; f1 -= box)
    {
        fprintf(out, "\n%5li.%03li ", (long)f1, taDecimals(f1, 3));
        for (i1 = 0; i1 < pnfbars->size; i1++)
        {
            if (taArrayItem(pnfbars->lo, i1) <= f1 + tenth && taArrayItem(pnfbars->hi, i1) >= f1 - tenth)
            {
                if (taArrayItem(pnfbars->oi, i1) > 0)
                    fprintf(out, "X");
                else
                    fprintf(out, "O");
            }
            else
                fprintf(out, " ");
        }
    }
#ifndef _WINDOWS
    if (stricmp(outfile, "STDOUT"))
#endif
        fclose(out);
    return (0);
}


int DLL_EXPORT taReadDOPfile(char *filename, int *cols)
{
    int    i1 = 0, i2;
    FILE    *fp1;
    char    *p1, buffer[81];
    static char *fields[8] = {"DATE", "TIME", "OPEN", "HIGH", "LOW", "CLOSE", "VOL", "OI"};

    if ((fp1 = fopen(filename, "rb")) == NULL)
        return (taErrFileOpen);
    memset(cols, 0, 8 * sizeof(int));
    while (fgets(buffer, 80, fp1))
    {
        for (i2 = strlen(buffer) - 1; (buffer[i2] == 0x0D || buffer[i2] == 0x0A || buffer[i2] == 0x1A) && i2 >= 0; i2--)
            buffer[i2] = '\0';
        if (buffer[0] == '\0')
            continue;
        p1 = strtok(buffer, ",");
        i1++;
        for (i2 = 0; i2 < 8; i2++)
        {
            if (!stricmp(p1, fields[i2]))
            {
                cols[i2] = i1;
                break;
            }
        }
    }
    fclose(fp1);
    return (0);
}


int DLL_EXPORT taTXT2Array(char *infile, taArray *a1, char *delimit, int skip, int startcol, int stopcol)
{
    int    i1 = 0, pos = 0, recno;
    char     buffer[256], *p1;
    FILE    *fp1;

    if ((fp1 = fopen(infile, "rt")) == NULL)
        return (taErrFileOpen);
    for (recno = 0; fgets(buffer, 255, fp1) != NULL; recno++)
        if (buffer[0] != '\n' && recno >= skip)
            i1++;
    stopcol--;
    startcol--;
    if (taAllocArrayM(a1, stopcol - startcol + 1, i1))
    {
        fclose(fp1);
        return (taErrMalloc);
    }
    fseek(fp1, 0L, SEEK_SET);
    for (recno = 0; fgets(buffer, 255, fp1) != NULL; recno++)
    {
        if (buffer[0] != '\n' && recno >= skip)
        {
            for (i1 = 0; i1 <= stopcol; i1++)
            {
                if (i1 == 0)
                    p1 = strtok(buffer, delimit);
                else
                    p1 = strtok(NULL, delimit);
                if (i1 >= startcol && i1 <= stopcol)
                    taArrayItemPM(a1, i1 - startcol, pos) = atof(p1);
            }
            pos++;
        }
    }
    fclose(fp1);
    return (0);
}


#if !defined(_WINDOWS) || defined(_WIN32)
int DLL_EXPORT taArrayPrintf(char *outfile, int append, int start, int stop, char *heading, char *format, ...)
{
    int    i1, pos, done = 0, maxsize = 0, line = 0, page = 0;
    char     buffer[256], datebuffer[12], *p1, sep[2];
    char    *dtformat[6] = {"YMD", "CYMD", "MDY", "MDCY", "DMY", "DMCY"};
    va_list  marker;
    taArray  a1;
    taArrayI ai1;
    FILE    *out;

#ifndef _WINDOWS
    if (!stricmp(outfile, "STDOUT"))
    {
        out = stdout;
        if (append)
            page = 1;
    }
    else
#endif
    {
        if (append)
            out = fopen(outfile, "at");
        else
            out = fopen(outfile, "wt");
        if (out == NULL)
            return (taErrFileOpen);
    }
    if (heading)
    {
        fprintf(out, heading);
        for (i1 = 0; heading[i1]; i1++)
            if (heading[i1] == '\n')
                line++;
    }
    p1 = format;
    va_start(marker, format);
    while (format[0])
    {
        i1 = strcspn(format, "%");
        if (i1)
        {
            format += i1;
            if (!format[0])
                break;
        }
        if (format[1] == '%')
        {
            format += 2;
            continue;
        }
        i1 = 1 + strcspn(format, "firu");
        strncpy(buffer, format, i1);
        buffer[i1] = '\0';
        format += i1;
        switch(buffer[i1 - 1])
        {
        case 'd':
        case 'i':
        case 'u':
            ai1 = va_arg(marker, taArrayI);
            maxsize = max(maxsize, ai1.size);
            break;
        case 'f':
            a1 = va_arg(marker, taArray);
            maxsize = max(maxsize, a1.size);
            break;
        }
    }
    stop = min(stop, maxsize - 1);
    for (pos = start; pos <= stop; pos++)
    {
#ifndef _WINDOWS
        line++;
        if (page && line == 25)
        {
            printf("Press any key to continue . . .");
            getchar();
            printf("\r                               \r");
            line = 1;
        }
#endif
        format = p1;
        va_start(marker, format);
        while (format[0])
        {
            i1 = strcspn(format, "%");
            if (i1)
            {
                strncpy(buffer, format, i1);
                buffer[i1] = '\0';
                fprintf(out, buffer);
                format += i1;
                if (!format[0])
                    break;
            }
            if (format[1] == '%')
            {
                fprintf(out, "%%");
                format += 2;
                continue;
            }
            i1 = 1 + strcspn(format, "firu");
            strncpy(buffer, format, i1);
            buffer[i1] = '\0';
            format += i1;

            switch(buffer[i1 - 1])
            {
            case 'd':
            case 'i':
            case 'u':
                ai1 = va_arg(marker, taArrayI);
                if (pos >= ai1.size)
                    continue;
                done = 0;
                for (i1 = 0; i1 < 6; i1++)
                {
                    if (!strncmp(buffer + 1, dtformat[i1], 3))
                    {
                        sep[0] = buffer[strlen(dtformat[i1]) + 1];
                        sep[1] = '\0';
                        if (sep[0] == 'i' || sep[0] == 'u')
                            i1 += 6;
                        taJulianToDateTxt(taArrayItem(ai1, pos), i1 + 1, sep, datebuffer);
                        fprintf(out, "%s", datebuffer);
                        done = 1;
                        break;
                    }
                }
                if (done)
                    break;
                if (toupper(buffer[1]) == 'H' && toupper(buffer[2]) == 'M')
                {
                    if (toupper(buffer[3]) == 'S')
                    {
                        i1 = 0;
                        sep[0] = buffer[4];
                    }
                    else
                    {
                        i1 = 1;
                        sep[0] = buffer[3];
                    }
                    sep[1] = '\0';
                    if (sep[0] == 'i' || sep[0] == 'u')
                        i1 += 2;
                    taJulianToTimeTxt(taArrayItem(ai1, pos), i1 + 1, sep, datebuffer);
                    fprintf(out, "%s", datebuffer);
                    break;
                }
                fprintf(out, buffer, taArrayItem(ai1, pos));
                break;

            case 'f':
                a1 = va_arg(marker, taArray);
                if (pos >= a1.size)
                    continue;
                fprintf(out, buffer, taArrayItem(a1, pos));
                break;

            case 'r':
                buffer[i1 - 1] = 'i';
                fprintf(out, buffer, pos);
                break;
            }
        }
    }
    va_end(marker);
#ifndef _WINDOWS
    if (stricmp(outfile, "STDOUT"))
#endif
        fclose(out);
    return (0);
}
#endif
