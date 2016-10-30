/* Technical Analysis Programmer's Toolkit - Copyright (C) 1997 KTA */

#ifndef __TADJ_H__
#define __TADJ_H__

#ifdef __cplusplus
extern "C" {
#endif


#pragma pack(1)

#define taDJBASEDATE 14976

struct taDJinfohdr
{
 char  filler1[45];
 short reccnt;
 char  filler2[24];
};

struct taDJinfo
{
 char     symbol[8];  /* symbol used by the Market Analyzer PLUS                   */
 char     equitytype; /* equity type: S stock, M mutual fund, O option, B bond,    */
                      /*              T treasury, W Tradeline, X DJ index,         */
                      /*              V commodity, Y user index, Z other           */
 char     decimals;   /* number of decimal places displayed                        */
                      /*        ASCII 46=0 47=1 40=2 49=3 50=4 places              */
 char     content;    /* data content '1' close                                    */
                      /*              '2' close,volume                             */
                      /*              '3' high,low,close                           */
                      /*       '0' or '4' high,low,close,volume                    */
                      /*              '5' high,low,close,open                      */
                      /*              '6' high,low,close,open,open interest        */
                      /*              '7' high,low,close,open,volume               */
                      /*              '8' high,low,close,open,open interest,volume */
                      /*              '9' all contracts volume and open interest   */
 char     filler1;    /* reserved                                                  */
 unsigned short begindt;/* Julian beginning date (0=Jan 1, 1920)                   */
 unsigned short enddt;/* Julian ending date for the data in the historical file    */
 float    lastclose;  /* last closing price as real number                         */
 char     company[15];/* company name ASCII                                        */
 char     filler2[10];/* reserved                                                  */
 short    reccnt;     /* integer number of units of data in the historical file    */
 char     datatype;   /* data type: D daily, W weekly, M monthly                   */
 char     tag[3];     /* 3 byte tag ASCII                                          */
 char     filler3[8]; /* reserved                                                  */
 char     avail[2];   /* available for use                                         */
 char     cusip[9];   /* Cusip   ASCII                                             */
 char     filler4;    /* reserved                                                  */
};

struct taDJqwik
{
 char     symbol[8];  /* program ticker symbol                                     */
 char     datatype;   /* data type: D daily, W weekly, M monthly                   */
 char     equitytype; /* equity type: S stock, M mutual fund, O option, B bond,    */
                      /*              T treasury, W Tradeline, X DJ index,         */
                      /*              V commodity, Y user index, Z other           */
 short    recno;      /* record in MAPINFO.DAT that belongs to this ticker         */
};

struct taDJdc
{
 unsigned short dt;
 float cl;
};
struct taDJdvc
{
 unsigned short dt;
 long vol;
 float cl;
};
struct taDJdhlc
{
 unsigned short dt;
 float hi, lo, cl;
};
struct taDJdvhlc
{
 unsigned short dt;
 long vol;
 float hi, lo, cl;
};
struct taDJdhlco
{
 unsigned short dt;
 float hi, lo, cl, op;
};
struct taDJdihlco
{
 unsigned short dt;
 long oi;
 float hi, lo, cl, op;
};
struct taDJdvhlco
{
 unsigned short dt;
 long vol;
 float hi, lo, cl, op;
};
struct taDJdvihlco
{
 unsigned short dt;
 long oi;
 long vol;
 float hi, lo, cl, op;
};
struct taDJdiv
{
 unsigned short dt;
 long oi;
 long vol;
};

#pragma pack()


#ifdef __cplusplus
 }
#endif
#endif
