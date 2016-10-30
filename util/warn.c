/********************************************************
 *							*
 *	Technical Analysis Trade System 		*
 *							*
 *		(C) Copyright				*
 *			Jesse Kuang			*
 *							*
 *							*
 *	$Revision$              *
 *	$Log$
 *							*
 *							*
 *							*
 ********************************************************/

#include "ktadefs.h"
#include <stdarg.h>
#ifdef	unix
#include <syslog.h>
#endif


#ifdef	unix
static	int	log_opened;
#endif


#ifndef WINDOWS
#define wvsprintf	vsprintf
#endif


#ifndef _WINDOWS
void OutputDBStr(LPSTR str)
{
    puts(str);
}
#else
void OutputDBStr(LPSTR);
#endif



void	warn(LPSTR fmt, ...)
{
    BYTE s[256];
    va_list marker;

    va_start(marker, fmt);
    wvsprintf(s, fmt, marker);
    va_end(marker);
#if	defined(WINDOWS) || defined(unix)
    strcat(s, "\n");
#else
    strcat(s, "\r\n");
#endif
    OutputDBStr(s);
}


#ifdef	unix

void	my_log(int err, LPSTR fmt, ...)
{
    BYTE s[256];
    int len;
    va_list marker;

    va_start(marker, fmt);
    len = wvsprintf(s, fmt, marker);
    va_end(marker);
    if (log_opened == 0) {
	openlog("KTA Broad", 0, LOG_USER);
	log_opened = 1;
    }
    syslog((err!=0)?LOG_DEBUG:LOG_INFO, "%s", s);
}
#endif
