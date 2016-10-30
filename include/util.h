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

#ifndef __KUTIL_H__
#define __KUTIL_H__

#ifndef __KTADEFS_H__
#include "ktatypes.h"
#endif


#ifdef __cplusplus
extern "C" {            /* Assume C declarations for C++ */
#endif  /* __cplusplus */


#ifdef	unix
#define strlwr		mystrlwr
#endif


// Following functions in c(l)utils.lib
//	CRC-32 Interface
ulg	updcrc(uch *s, unsigned n);
/* String manipulation	*/
//	strtok
char	*mystrtok(char *, char *);
int	skw_match( char *str, char *kw );
int	str_match(char *str, char *kw);
char	*mystrlwr(char *);
//	build full path
char	*build_path(char *filep);
//	Change Extension, return static buffer pointer
char	*chgext(const char *path, const char *newext);
char	*chgext_b(const char *path, const char *newext, char *newpath);
LPSTR	mytrim_n(LPSTR str, int num_chars);


/* Time subroutine	*/
#define UNIX_ADJUSTMENT 	2440588L

struct	mytm	{
int		year;
UINT	month;	// 1-12
UINT	day;	// 1-31
};

//	convert string to long date
dword	str2date(LPSTR format, LPSTR str);	// format "YYYYMMDD", "YYMMDD", "MMDD"
//	convert years/months/day to julian long date
dword	ymd2date(int years, int months, int day);
struct	mytm	*kmdate(dword _date);
struct	mytm	*kmdate_b(dword _date, struct mytm *);
LPSTR	Time2String(dword timev);
LPSTR	Time2String_d(dword timev);
LPSTR	Time2String_b(dword timev, LPSTR tbuff);
void	date2str(dword _date, LPSTR bbuf);
dword	End_Month(dword);
int	days_of_year(dword tot_d);


#ifdef __cplusplus
}                       /* End of extern "C" { */
#endif  /* __cplusplus */

#endif
