/********************************************************
 *														*
 *	Technical Analysis Trade System						*
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

#ifndef __KTATYPES_H__
#define __KTATYPES_H__


#if	!defined(WINDOWS) && (defined(_WIN32) || defined(_WIN64))
#define WINDOWS
#endif
#ifdef	WINDOWS
#include <windows.h>
#endif	// WINDOWS
#ifdef	unix
#include <sys/types.h>
#endif

#if	defined(__GNUC__) && !defined(aix)
#define __version_data	__attribute__ ((unused,__section__(".version.data")))
#else
#define __version_data
#endif

#include <limits.h>

/* Identify if 64 bits platform with __64BIT__.
 * Can also be done from compiler command line.
 */
#if defined(_WIN64)
 #define __64BIT__ 1
#endif

#if defined( __LP64__ ) || defined( _LP64 )
 #define __64BIT__ 1
#endif

/* Check also for some known GCC def for 64 bits platform. */
#if defined(__alpha__) ||defined(__ia64__) ||defined(__ppc64__) ||defined(__s390x__) ||defined(__x86_64__)
 #define __64BIT__ 1
#endif

#if !defined(__MACTYPES__)
typedef signed int   Int32;
typedef unsigned int UInt32;

    #if defined(_WIN32) || defined(_WIN64)
           /* See "Windows Data Types". Platform SDK. MSDN documentation. */
    typedef signed __int64   Int64;
    typedef unsigned __int64 UInt64;
    #else
        #if defined(__64BIT__)
        /* Standard LP64 model for 64 bits Unix platform. */
        typedef signed long   Int64;
        typedef unsigned long UInt64;
        #else
        /* Standard ILP32 model for 32 bits Unix platform. */
        typedef signed long long   Int64;
        typedef unsigned long long UInt64;
        #endif
    #endif
#endif

#if	!defined(_mysql_h) || MYSQL_VERSION_ID >= 32210
typedef unsigned char	byte;
#endif
typedef unsigned short  word;
typedef unsigned long	dword;
typedef UInt64          qword;
#define ulg				dword
typedef short			int16;
typedef long			int32;
typedef void			*lpvoid;
#ifndef __cplusplus
typedef int				bool;
#endif

/* Following for Win Compatible defines */
#ifndef WINDOWS
#define UINT            unsigned
#define WORD			word
#define DWORD			dword
#define LPVOID			lpvoid
#define BYTE			byte
#define BOOL			int
#define LONG			long
#define HWND			unsigned
#define HINSTANCE		unsigned
#define WINAPI
typedef char			*LPSTR;
#endif

#ifndef TRUE
#define TRUE	(-1)
#endif
#ifndef FALSE
#define FALSE	0
#endif

#endif	// __KTATYPES_H__
