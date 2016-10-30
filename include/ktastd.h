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

#ifndef __KTASTD_H__
#define __KTASTD_H__


#include <string.h>
#include <memory.h>
#include <stdio.h>
#include <time.h>
#include <fcntl.h>
#include <math.h>
#include <stdlib.h>
#include <sys/stat.h>

#ifdef  unix
//#include <limits.h>
#include <sys/time.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <time.h>
#include <errno.h>
#if	defined(USE_THREAD) && !defined(WINDOWS)
#include <pthread.h>
#endif
#else	// not unix
#include <io.h>
#include <share.h>
#endif

#if     defined(MEMWATCH)
#include "memwatch.h"
#endif

#ifdef  __GNUC__
#undef  min
#define min(X,Y)  __extension__ ({  typeof(X) __x=(X), __y=(Y); (__x<__y)?__x:__y; })

#undef  max
#define max(X,Y)  __extension__ ({ typeof(X) __x=(X), __y=(Y); (__x>__y)?__x:__y; })

#endif // __GNUC__

#ifdef __cplusplus
extern "C" {            /* Assume C declarations for C++ */
#endif  /* __cplusplus */


#ifndef MAXPATH
#define MAXPATH 	128
#endif


#ifndef	offsetof
#define	offsetof(typ, memb)	((size_t) &(((typ *)0)->memb) )
#endif


#if defined(unix) || defined(linux)
#define wsprintf	sprintf
#define wvsprintf	vsprintf
#endif // WINDOWS

#if !defined(WINDOWS) && !defined(WIN32)
#define GetPrivateProfileInt(s,k,d,p)	GetConfigInt(p,s,k,d)
#define GetPrivateProfileString(s,k,d,r,siz,p)	GetConfigString(p,s,k,d,r,siz)
#define	WritePrivateProfileString(s,k,buf,p)	WriteConfigString(p,s,k,buf)
extern	int		GetMemoryInt(char *, char *, char *, int);
extern	int		GetConfigInt(char *,		/* filespec */
				     char *,		/* section */
				     char *,		/* key */
				     int);		/* default */

extern	int		GetMemoryString(char *, char *, char *, char *,
				char *, unsigned int);
extern	int		GetConfigString(char *, 	/* filespec */
					char *, 	/* section */
					char *, 	/* key */
					char *, 	/* default */
					char *, 	/* buffer */
					unsigned int);	/* length */

extern	int		WriteConfigString(char *, char *, char *, char *);

extern	int		GetMemorySection(char *, char *, char *,
				unsigned int);
extern	int		GetConfigSection(char *,	/* filespec */
					 char *,	/* section */
					 char *,	/* buffer */
					 unsigned int); /* length */

extern	int		GetMemoryKeys(char *, char *, char *, unsigned int);
extern	int		GetConfigKeys(char *,		/* filespec */
				      char *,		/* section */
				      char *,		/* buffer */
				      unsigned int);	/* length */

extern	int		gbConfigCloseFileOnExit;	/* default: NO	*/
extern	int		gbConfigDeleteBakOnExit;	/* default: YES */
#endif

#if	!defined(unix) && !defined(WIN32)
ush	WINAPI my_ntohs( ush a );
ush WINAPI my_htons( ush a );
ulg	WINAPI my_ntohl( ulg x );
ulg	WINAPI my_htonl( ulg x );
#ifdef	__WATCOMC__
#pragma aux my_ntohs =	\
	"xchg    al,ah" \
	parm [ax]	\
	value [ax];

#pragma aux my_htons =	\
	"xchg al, ah"   \
	parm [ax]	\
	value [ax];
#endif
#else
#define my_ntohs	ntohs
#define my_ntohl	ntohl
#define my_htons	htons
#define my_htonl	htonl
#endif



/* Diagnostic functions */
#ifdef DEBUG
#  define Assert(cond,msg) {if(!(cond)) error(msg);}
#  define Trace(x) fprintf x
#  define Tracev(x) {if (verbose) fprintf x ;}
#  define Tracevv(x) {if (verbose>1) fprintf x ;}
#  define Tracec(c,x) {if (verbose & c) fprintf x ;}
#  define Tracecv(c,x) {if (verbose>1 && (c)) fprintf x ;}
#else
#  define Assert(cond,msg)
#  define Trace(x)
#  define Tracev(x)
#  define Tracevv(x)
#  define Tracec(c,x)
#  define Tracecv(c,x)
#endif


#ifdef	unix
void    my_log(int err, LPSTR lpszFmt, ...);
#define stricmp         strcasecmp
#endif
#ifdef  WINDOWS
#define warn    WARN
void    FAR CDECL warn(LPSTR lpszFmt, ...);
#else
void	warn(LPSTR lpszFmt, ...);
#endif


#ifdef __cplusplus
}                       /* End of extern "C" { */
#endif  /* __cplusplus */

#endif	// __KTASTD_H__
