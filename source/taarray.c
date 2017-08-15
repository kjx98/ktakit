/* Technical Analysis Programmer's Toolkit - Copyright (C) 1997 KTA */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#ifdef  WINDOWS
#include <conio.h>
#endif
#include <stdarg.h>
#include "ktakit.h"

taArray  taNOARRAY = {NULL,0,0,0,0};
taArray *taNULLARRAY = &taNOARRAY;
taBars   taNOBARS = {{NULL,0,0,0,0}};
taBars  *taNULLBARS = &taNOBARS;

int DLL_EXPORT taAllocArray(taArray *a1, size_t size)
{
    a1->size = size;
    a1->width = 1;
    a1->element = 0;
    a1->type = 0;
    if ((a1->data = (KFloat *)calloc(size, sizeof(KFloat))) == NULL)
        return (taErrMalloc);
    return (0);
}

int DLL_EXPORT taAllocArrayI(taArrayI *a1, size_t size)
{
    a1->size = size;
    a1->width = 1;
    a1->element = 0;
    a1->type = 1;
    if ((a1->data = (int *)calloc(size, sizeof(int))) == NULL)
        return (taErrMalloc);
    return (0);
}

int DLL_EXPORT taAllocArrayM(taArray *a1, int width, size_t size)
{
    a1->size = size;
    a1->width = width;
    a1->element = 0;
    a1->type = 0;
    if ((a1->data = (KFloat *)calloc(size * width, sizeof(KFloat))) == NULL)
        return (taErrMalloc);
    return (0);
}

int DLL_EXPORT taAllocArrayIM(taArrayI *a1, int width, size_t size)
{
    a1->size = size;
    a1->width = width;
    a1->element = 0;
    a1->type = 1;
    if ((a1->data = (int *)calloc(size * width, sizeof(int))) == NULL)
        return (taErrMalloc);
    return (0);
}

int DLL_EXPORT taAllocBars(taBars *b1, size_t size)
{

//b1->path[0] = '\0';
//b1->name[0] = '\0';
    b1->size = size;
    b1->datasize = size;
    b1->reccnt = 0;
//b1->type = 0;
    if ((b1->op.data = (KFloat *)calloc(size, SIZEOFBARDATA)) != NULL)
    {
#ifdef  KFLOAT_DOUBLE
        b1->op.size = size;
        b1->op.width = 8;
        b1->op.element = 1;
        b1->op.type = 0;
        b1->hi = b1->lo = b1->cl = b1->vol = b1->turnov = b1->op;
        b1->hi.element = 2;
        b1->lo.element = 3;
        b1->cl.element = 4;
        b1->vol.element = 5;
        b1->turnov.element = 6;
        b1->dt.data = (int *)b1->op.data;
        b1->dt.size = size;
        b1->dt.width = 16;
        b1->dt.element = 0;
        b1->dt.type = 1;
        b1->ti = b1->oi = b1->sprd = b1->dt;
        b1->ti.element = 1;
        b1->oi.element = 14;
        b1->sprd.element = 15;
#else
        b1->op.size = size;
        b1->op.width = 10;
        b1->op.element = 2;
        b1->op.type = 0;
        b1->hi = b1->lo = b1->cl = b1->vol = b1->turnov = b1->op;
        b1->hi.element = 3;
        b1->lo.element = 4;
        b1->cl.element = 5;
        b1->vol.element = 6;
        b1->turnov.element = 7;
        b1->dt.data = (int *)b1->op.data;
        b1->dt.size = size;
        b1->dt.width = 10;
        b1->dt.element = 0;
        b1->dt.type = 1;
        b1->ti = b1->oi = b1->sprd = b1->dt;
        b1->ti.element = 1;
        b1->oi.element = 8;
        b1->sprd.element = 9;
#endif
    }
    return (0);
}

long DLL_EXPORT taDecimals(KFloat value, int places)
{
    double   d1;
    return (long)fabs((modf(value, &d1) * pow(10, places)));
}

void DLL_EXPORT taFreeArray(taArray *a1)
{
    free(a1->data);
    a1->data = NULL;
    return;
}

void DLL_EXPORT taFreeArrayI(taArrayI *a1)
{
    free(a1->data);
    a1->data = NULL;
    return;
}

void DLL_EXPORT taFreeBars(taBars *b1)
{
    free(b1->dt.data);
    memset(b1, 0, sizeof(taBars));
    return;
}

KFloat DLL_EXPORT taGetArrayItem(taArray *a1, unsigned int pos)
{
    if (pos >= a1->size) return (0);
    if (a1->type == 0)
    {
        return (taArrayItemP(a1, pos));
    }
    else
    {
        return (taArrayItemP(((taArrayI *)a1), pos));
    }
}

KFloat DLL_EXPORT taGetArrayItemM(taArray *a1, int element, unsigned int pos)
{
    if (pos >= a1->size) return (0);
    if (a1->type == 0) return (taArrayItemPM(a1, element, pos));
    else
        return (taArrayItemPM(((taArrayI *)a1), element, pos));
}

int DLL_EXPORT taIEEEtoMS(float *ieee, float *ms)
{
    unsigned int sign;
    struct tafloatbytes
    {
        unsigned char bl, bh, al, ah;
    } temp;

    if (*ieee == 0)
    {
        memset(ms, 0, 4);
        return(0);
    }
    memcpy(&temp, (char *)ieee, 4);
    sign = temp.ah & 128;
    temp.ah <<= 1;
    temp.ah += ((temp.al & 128) >> 7);
    temp.al = (temp.al & 127) + sign;
    temp.ah += 2;
    memcpy((char *)ms, &temp, 4);
    return(0);
}

int DLL_EXPORT taMStoIEEE(float *ms, float *ieee)
{
    unsigned int sign;
    struct tafloatbytes
    {
        unsigned char bl, bh, al, ah;
    } temp;

    memcpy(&temp, (char *)ms, 4);
    if (temp.ah == 0)
    {
        memset(ieee, 0, 4);
        return (0);
    }
    temp.ah -= 2;
    sign = temp.al & 128;
    temp.al -= sign + ((temp.ah & 1) << 7);
    temp.ah >>= 1;
    temp.ah += sign;
    memcpy((char *)ieee, &temp, 4);
    return (0);
}

int DLL_EXPORT taReAllocBars(taBars *b1, size_t size)
{
    void    *temp;

    if (b1->datasize < size)
    {
        {
            if ((unsigned int) size * SIZEOFBARDATA >= 0x80000000)
                return (taErrMalloc);
            temp = b1->op.data;
            if ((b1->op.data = (KFloat *)realloc(b1->op.data, size * SIZEOFBARDATA)) == NULL)
            {
                b1->op.data = temp;
                return (taErrMalloc);
            }
        }
        b1->datasize = size;
    }
    taReSizeBars(b1, size);
    return (0);
}

int DLL_EXPORT taReSizeBars(taBars *b1, size_t size)
{
    if (size > b1->datasize) return (taErrMalloc);
    b1->size = size;
    b1->dt.size = size;
    b1->cl.size = size;
    b1->ti.size = size;
    b1->op.size = size;
    b1->hi.size = size;
    b1->lo.size = size;
    b1->vol.size = size;
    b1->turnov.size = size;
    b1->oi.size = size;
    b1->sprd.size = size;
    return (0);
}

void DLL_EXPORT taSetArrayItem(taArray *a1, unsigned int pos, KFloat value)
{
    if (pos >= a1->size) return;
    if (a1->type == 0)
        taArrayItemP(a1, pos) = value;
    else
        taArrayItemP(((taArrayI *)a1), pos) = (int)value;
    return;
}

void DLL_EXPORT taSetArrayItemM(taArray *a1, int element, unsigned int pos, KFloat value)
{
    if (pos >= a1->size) return;
    if (a1->type == 0)
        taArrayItemPM(a1, element, pos) = value;
    else
        taArrayItemPM(((taArrayI *)a1), element, pos) = (int)value;
    return;
}

int DLL_EXPORT taErrDesc(int err, char *buffer)
{
    static char *errdesc[16] =
    {
        "Sucessfull",
        "Invalid Parm",
        "File Open",
        "Malloc",
        "File Read",
        "File Write",
        "File Seek",
        "File Exists",
        "File Not Found",
        "Record Not Found",
        "No Graphics Found",
        "No Fonts Found",
        "Chart Uninitialized",
        "Max Lines Exceeded",
        "Max Grids Exceeded",
        "Unknown"
    };
    if (err > 0 || err < -14) err = -15;
    strcpy(buffer, errdesc[-err]);
    return (strlen(buffer));
}

void taFree(void *buffer)
{
#if !defined(_WINDOWS)
    free(buffer);
#elif defined(_WIN32)
    free(buffer);
#else
    HGLOBAL  hglb;
    hglb = LOWORD(GlobalHandle(SELECTOROF(buffer)));
    GlobalUnlock(hglb);
    GlobalFree(hglb);
#endif
    return;
}

void *taMalloc(unsigned int size)
{
#if !defined(_WINDOWS)
    return (malloc(size));
#elif defined(_WIN32)
    return (malloc(size));
#else
    HGLOBAL  hglb;
    void FAR *lpvBuffer;
    hglb = GlobalAlloc(GHND, size);
    if (hglb == 0)
        return (NULL);
    lpvBuffer = GlobalLock(hglb);
    return (lpvBuffer);
#endif
}


#ifdef _WINDOWS

int        fprintf(FILE * stream, const char *format,...)
{
    int      r1;
    char     text[1024];
    va_list  marker;

    va_start(marker, format);
    r1 = wvsprintf(text, format, marker);
    va_end(marker);
    fputs(text, stream);
    return (r1);
}

int        sprintf(char *text, const char *format,...)
{
    int      r1;
    va_list  marker;

    va_start(marker, format);
    r1 = wvsprintf(text, format, marker);
    va_end(marker);
    return (r1);
}

#endif
