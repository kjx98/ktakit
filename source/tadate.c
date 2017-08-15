/* Technical Analysis Programmer's Toolkit - Copyright (C) 1997 KTA */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "ktakit.h"


unsigned int DLL_EXPORT taDateTxtDif(char *date1, char *date2, int format1, int format2)
{
    return (abs(taDateTxtToJulian(date1, format1) - taDateTxtToJulian(date2, format2)));
}

unsigned int DLL_EXPORT taDateTxtToJulian(char *date, int format)
{
int     cent, year, month, day;
char    buffer[3] = "00";
int cols[12][4] = {
        -1, 0, 3, 6,  0, 2, 5, 8,
        -1, 6, 0, 3,  6, 8, 0, 3,
        -1, 6, 3, 0,  6, 8, 3, 0,
        -1, 0, 2, 4,  0, 2, 4, 6,
        -1, 4, 0, 2,  4, 6, 0, 2,
        -1, 4, 2, 0,  4, 6, 2, 0};

    if(date == NULL) return(0);
    while (date[0] == ' ' ) date++;
    if(date[0] == '\"') date++;
    if(date[0] == '\0' || date[0] == '\"') return(0);
    if (format < 1 || format > 12) return(0);
    format--;
    buffer[0] = date[cols[format][1]];
    buffer[1] = date[cols[format][1] + 1];
    year = atoi(buffer);
    if (cols[format][0] >= 0)
    {
        buffer[0] = date[cols[format][0]];
        buffer[1] = date[cols[format][0] + 1];
        cent = 100 * (atoi(buffer) - 19);
    }
    else
    {
        if (year < 61) cent = 2000;
        else cent = 1900;
    }
    buffer[0] = date[cols[format][2]];
    buffer[1] = date[cols[format][2] + 1];
    month = atoi(buffer);
    buffer[0] = date[cols[format][3]];
    buffer[1] = date[cols[format][3] + 1];
    day = atoi(buffer);
    if (!year && !month && !day) return (0);
    return (taYMDToJulian(year + cent, month, day));
}

int DLL_EXPORT taDateTxtVal(char *date, int format)
{
unsigned int cent, year, month, day;
char        buffer[3] = "00";
unsigned    days[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
int         cols[12][4] = {
                -1, 0, 3, 6,  0, 2, 5, 8,
                -1, 6, 0, 3,  6, 8, 0, 3,
                -1, 6, 3, 0,  6, 8, 3, 0,
                -1, 0, 2, 4,  0, 2, 4, 6,
                -1, 4, 0, 2,  4, 6, 0, 2,
                -1, 4, 2, 0,  4, 6, 2, 0};

    if(date == NULL) return(0);
    while (date[0] == ' ' ) date++;
    if(date[0] == '\"') date++;
    if(date[0] == '\0' || date[0] == '\"') return(0);
    if (format < 1 || format > 12) return(taErrInvalidParm);
    format--;
    buffer[0] = date[cols[format][1]];
    buffer[1] = date[cols[format][1] + 1];
    year = atoi(buffer);
    if (cols[format][0] >= 0)
    {
        buffer[0] = date[cols[format][0]];
        buffer[1] = date[cols[format][0] + 1];
        cent = 100 * (atoi(buffer) - 19);
    }
    else
    {
        if (year < 61) cent = 2000; else cent = 1900;
    }
    buffer[0] = date[cols[format][2]];
    buffer[1] = date[cols[format][2] + 1];
    month = atoi(buffer);
    buffer[0] = date[cols[format][3]];
    buffer[1] = date[cols[format][3] + 1];
    day = atoi(buffer);
    if (!year && !month && !day) return (taErrInvalidParm);
    if (cent + year < 1961 || cent + year > 2140) return (taErrInvalidParm);
    if (month > 12) return (taErrInvalidParm);
    if (year % 4 == 0) days[1] = 29;
    if (day > days[month - 1]) return (taErrInvalidParm);
    return (0);
}

unsigned int DLL_EXPORT taHMSDif(int hr1, int min1, int sec1, int hr2, int min2, int sec2)
{
    return (abs(taHMSToJulian(hr1, min1, sec1) - taHMSToJulian(hr2, min2, sec2)));
}

unsigned int DLL_EXPORT taHMSToJulian(int hr, int min, int sec)
{
    return (hr * 1800 + min * 30 + sec / 2);
}

unsigned int DLL_EXPORT taJulianDif(unsigned int dti1, unsigned int dti2)
{
    return (abs(dti1 - dti2));
}

int DLL_EXPORT taJulianDOW(unsigned int date)
{
    return ((date - 1) % 7);
}

void DLL_EXPORT taJulianToDateTxt(unsigned int date, int format, char *sep, char *buffer)
{
int cent = 0, year = 0, month = 0, day = 0;

    if (date)
    {
        taJulianToYMD(date, &year, &month, &day);
        cent = year / 100;
        year -= cent * 100;
    }
    switch (format)
    {
    case DTFORMAT_YY_MM_DD:
        sprintf(buffer, "%.2i%c%.2i%c%.2i", year, sep[0], month, sep[0], day);
        break;
    case DTFORMAT_CCYY_MM_DD:
        sprintf(buffer, "%.2i%.2i%c%.2i%c%.2i", cent, year, sep[0], month, sep[0], day);
        break;
    case DTFORMAT_MM_DD_YY:
        sprintf(buffer, "%.2i%c%.2i%c%.2i", month, sep[0], day, sep[0], year);
        break;
    case DTFORMAT_MM_DD_CCYY:
        sprintf(buffer, "%.2i%c%.2i%c%.2i%.2i", month, sep[0], day, sep[0], cent, year);
        break;
    case DTFORMAT_DD_MM_YY:
        sprintf(buffer, "%.2i%c%.2i%c%.2i", day, sep[0], month, sep[0], year);
        break;
    case DTFORMAT_DD_MM_CCYY:
        sprintf(buffer, "%.2i%c%.2i%c%.2i%.2i", day, sep[0], month, sep[0], cent, year);
        break;
    case DTFORMAT_YYMMDD:
        sprintf(buffer, "%.2i%.2i%.2i", year, month, day);
        break;
    case DTFORMAT_CCYYMMDD:
        sprintf(buffer, "%.2i%.2i%.2i%.2i", cent, year, month, day);
        break;
    case DTFORMAT_MMDDYY:
        sprintf(buffer, "%.2i%.2i%.2i", month, day, year);
        break;
    case DTFORMAT_MMDDCCYY:
        sprintf(buffer, "%.2i%.2i%.2i%.2i", month, day, cent, year);
        break;
    case DTFORMAT_DDMMYY:
        sprintf(buffer, "%.2i%.2i%.2i", day, month, year);
        break;
    case DTFORMAT_DDMMCCYY:
        sprintf(buffer, "%.2i%.2i%.2i%.2i", day, month, cent, year);
    break;
    default:
        buffer[0] = '\0';
        break;
    }
    return;
}

void DLL_EXPORT taJulianToHMS(unsigned int time, int *hr, int *min, int *sec)
{
    *hr = time / 1800;
    time %= 1800;
    *min = time / 30;
    *sec = (time % 30) * 2;
}

void DLL_EXPORT taJulianToTimeTxt(unsigned int time, int format, char *sep, char *buffer)
{
int hour = 0, min = 0, sec = 0;

    if (time) taJulianToHMS(time, &hour, &min, &sec);

    switch (format)
    {
    case TIFORMAT_HH_MM_SS:
        sprintf(buffer, "%.2i%c%.2i%c%.2i", hour, sep[0], min, sep[0], sec);
        break;
    case TIFORMAT_HH_MM:
        sprintf(buffer, "%.2i%c%.2i", hour, sep[0], min);
        break;
    case TIFORMAT_HHMMSS:
        sprintf(buffer, "%.2i%.2i%.2i", hour, min, sec);
        break;
    case TIFORMAT_HHMM:
        sprintf(buffer, "%.2i%.2i", hour, min);
        break;
    default:
        buffer[0] = '\0';
        break;
    }
    return;
}

void DLL_EXPORT taJulianToYMD(unsigned int date, int *year, int *month, int *day)
{
int daystable[12] = {0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334};
unsigned mm = 0, dd = 0, yy = 0;

    yy = (unsigned)((4 * date - 1) / 1461);
    dd = (unsigned)(date - ((1461 * yy) / 4));
    yy += 61;
    if (yy % 4 == 0) for (mm = 2; mm < 12; mm++) daystable[mm]++;
    for (mm = 11; mm >= 0 && dd <= daystable[mm]; mm--);
    dd -= daystable[mm];
    *year = yy + 1900;
    *month = mm + 1;
    *day = dd;
}

unsigned int DLL_EXPORT taTimeTxtDif(char *time1, char *time2, int format1, int format2)
{
    return (abs(taTimeTxtToJulian(time1, format1) - taTimeTxtToJulian(time2, format2)));
}

unsigned int DLL_EXPORT taTimeTxtToJulian(char *time, int format)
{
unsigned int hour, min, sec = 0;
char        buffer[3] = "00";
int         cols[4][3] = {0, 3, 6,   0, 3,-1,   0, 2, 4,   0, 2,-1};

    if(time == NULL) return(0);
    while (time[0] == ' ' ) time++;
    if(time[0] == '\"') time++;
    if(time[0] == '\0' || time[0] == '\"') return(0);
    if (format < 1 || format > 4) return(0);
    format--;
    buffer[0] = time[cols[format][0]];
    buffer[1] = time[cols[format][0] + 1];
    hour = atoi(buffer);
    buffer[0] = time[cols[format][1]];
    buffer[1] = time[cols[format][1] + 1];
    min = atoi(buffer);
    if (cols[format][2] >= 0)
    {
        buffer[0] = time[cols[format][2]];
        buffer[1] = time[cols[format][2] + 1];
        sec = atoi(buffer);
    }
    if (!hour && !min && !sec) return (0);
    return (taHMSToJulian(hour, min, sec));
}

unsigned int DLL_EXPORT taYMDDif(int year1, int month1, int day1,
			int year2, int month2, int day2)
{
    return(abs(taYMDToJulian(year1, month1, day1) - taYMDToJulian(year2, month2, day2)));
}

unsigned int DLL_EXPORT taYMDToJulian(int year, int month, int day)
{
unsigned date, daystable[12] = {0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334};

    if (month == 0 || day == 0) return (0);
    if (year > 1900) year -= 1900;
    if (year < 61) year += 100;
    date = day + (1461L * (year - 61) / 4) + daystable[month - 1];
    if (month > 2 && year % 4 == 0) date++;
    return (date);
}
