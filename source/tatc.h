/* Technical Analysis Programmer's Toolkit - Copyright (C) 1997 KTA */

#ifndef __TATC_H__
#define __TATC_H__

#ifdef __cplusplus
extern "C" {
#endif

struct taTChdr
{
 char     name[22];
 char     filler[26];
};

struct taTCrec
{
 float    dt;
 float    hi;
 float    lo;
 float    cl;
 float    vol;
 float    op;
};


#ifdef __cplusplus
 }
#endif
#endif
