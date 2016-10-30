/* Technical Analysis Programmer's Toolkit - Copyright (C) 1997 KTA */

#ifndef __TAFS_H__
#define __TAFS_H__

#ifdef __cplusplus
extern "C" {
#endif

#pragma pack(1)

struct taFScontractspec
{
 char     filler[8];
};

struct taFScontractinfo
{
 char     exchange;
 char     fraction;
 char     multiplier;
 unsigned short fluctuation;
 short    factor;
};

struct taFSinfo  /* header */
{
 short    version;
 unsigned short dataoffset;
 unsigned short contractoffset;
 unsigned char numcontracts;
 unsigned short studyoffset;
 unsigned char numstudies;
 unsigned char studytype;
 unsigned char recordlen;
 char     filename[13];
 char     filetype;
 unsigned char minutes;
 unsigned char fraction;
 char     power;
 char     filler1;
 unsigned short voifileexists:1;
 unsigned short bit32values:1;
 unsigned short signeddata:1;
 unsigned short autoupdatenorm:1;
 unsigned short autoupdatevoi:1;
 unsigned short archived:1;
 short    timezone;
 unsigned short expirationdate;
 unsigned char daymap1;
 unsigned short start1;
 unsigned short end1;
 unsigned char daymap2;
 unsigned short start2;
 unsigned short end2;
 char     filler2[10];
 char     symbolname[4];
 char     symboltype;
 struct taFScontractspec contractspec;
 struct taFScontractinfo contractinfo1;
 struct taFScontractinfo contractinfo2;
 struct taFScontractinfo contractinfo3;
 char     desc[28];
};

struct taFSdaily16
{
 unsigned short date, open, high, low, close;
};

struct taFSvar16
{
 unsigned short date, time, open, high, low, close;
};

struct taFStick16
{
 unsigned short date, time, close;
};

struct taFSvoi16
{
 unsigned short date, vol, oi;
 unsigned short filler1:1;
 unsigned short volexp:2;
 unsigned short oiexp:2;
 unsigned short filler2:3;
};

struct taFSdaily32
{
 unsigned short date;
 unsigned long open, high, low, close;
};

struct taFSvar32
{
 unsigned short date, time;
 unsigned long open, high, low, close;
};

struct taFStick32
{
 unsigned short date, time;
 unsigned long close;
};

struct taFSvoi32
{
 unsigned short date;
 unsigned long vol, oi;
};

struct taFStablehdr
{
 unsigned short version;
 unsigned short recordlen;
};

struct taFSfracs
{
 char     shortname[8];
 char     longname[32];
 unsigned short xmitdenom;
 unsigned short dispdenom;
 unsigned char nondec;
 unsigned char bitmap;
 unsigned short divide;
 unsigned short multiply;
 unsigned short subtract;
 unsigned short add;
 unsigned short xmitpower;
 unsigned short disppower;
};

#pragma pack()


#ifdef __cplusplus
 }
#endif
#endif
