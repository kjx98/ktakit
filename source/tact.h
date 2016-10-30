/* Technical Analysis Programmer's Toolkit - Copyright (C) 1997 KTA */

#ifndef __TACT_H__
#define __TACT_H__

#ifdef __cplusplus
extern "C" {
#endif

#pragma pack(1)

struct taCTdathdr
{
 short    maxrecs;
 short    lastrec;
};

struct taCTmasthdr
{
 short    numentries;
 short    nextfilenum;
 char     filler[49];
};

struct taCTdata
{
 unsigned char filenbr;   /* File number          */
 short    filetype;       /* File type CT resrvd  */
 char     reclen;         /* DAT file rec length  */
 char     numfields;      /* DAT file num fields  */
 char     filler1[2];
 char     name[16];       /* Name of issue        */
 char     filler2;
 char     ctv28flag;      /* CT version 2.8 flag  */
 float    firstdate;      /* First date on file   */
 float    lastdate;       /* Last date on file    */
 char     dataformat;     /* I D W M Q Y          */
 short    idatime;        /* Base time            */
 char     datasvcuse[16]; /* For data service use */
 char     filler3;
};

struct taXmasterHdr
{
 char     filler1[10];
 long     numentries;
 long     numentries2;
 long     nextfilenum;
 char     copyright[128];
};

struct taXmaster
{
 char     filler1[1];     /* x01 */
 char     symbol[15];
 char     name[45];
 char     filler2[1];
 char     dataformat;
 short    tickinterval;
 short    filenbr;
 char     filler3[2];
 char     units;
 unsigned char hlcvd:5;
 unsigned char op:1;
 unsigned char oi:1;
 unsigned char ti:1;
 char     filler4[9];
 long     createdate;
 char     filler5[20];
 long     firstdate1;
 long     firstdate;
 long     firsttime;
 long     lastdate;
 long     lasttime;
 long     starttime;
 long     endtime;
 char     filler6[18];
};

#pragma pack()

#define sizeof_taCSIrec 32


#ifdef __cplusplus
 }
#endif
#endif
