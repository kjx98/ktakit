/* Technical Analysis Programmer's Toolkit - Copyright (C) 1997 KTA */

#ifndef __TATT_H__
#define __TATT_H__

#ifdef __cplusplus
extern "C" {
#endif

#pragma pack(1)

struct taTTrec
{
 unsigned short dt;
 unsigned short op;
 unsigned short hi;
 unsigned short lo;
 unsigned short cl;
 unsigned char  vol[3];
 unsigned char  oi[3];
};

#pragma pack()


#ifdef __cplusplus
 }
#endif
#endif
