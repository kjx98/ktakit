/********************************************************
 *														*
 *	Technical Analysis Trade System 					*
 *														*
 *		(C) Copyright									*
 *			Jesse Kuang									*
 *														*
 *														*
 *	$Revision$											*
 *	$Log$												*
 *														*
 *														*
 *														*
 ********************************************************/

#ifndef __KTADEFS_H__
#define __KTADEFS_H__


#ifndef __KTATYPES_H__
#include "ktatypes.h"
#endif

#ifndef __KTASTD_H__
#include "ktastd.h"
#endif


/* Following for CPU/Platform defines	*/
#ifdef	sparc
#define B_ENDIAN
#endif

#ifdef  DEBUG
#define DLL_EXPORT
#else
#if defined(BUILD_DLL) && !defined(unix)
#define DLL_EXPORT __declspec(dllexport) WINAPI
#else
//#define DLL_EXPORT __declspec(dllimport) APIENTRY
#define DLL_EXPORT WINAPI
#endif
#endif



#ifdef __cplusplus
extern "C" {            /* Assume C declarations for C++ */
#endif  /* __cplusplus */

#ifndef B_ENDIAN
// just like Pentium
#define myintels(x)	x
#define myintell(x)	x
#else
// just like sparc
word	myintels(word v);
dword	myintell(dword v);
#endif


#ifdef __cplusplus
}                       /* End of extern "C" { */
#endif  /* __cplusplus */


#endif	/*	__KTADEFS_H__	*/
