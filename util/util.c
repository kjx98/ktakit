/********************************************************
 *							*
 *	Technical Analysis Trade System 		*
 *							*
 *		(C) Copyright				*
 *			Jesse Kuang			*
 *							*
 *							*
 *	$Revision$              *
 *	$Log$                   *
 *							*
 *							*
 *							*
 ********************************************************/

#include "ktadefs.h"
#include <ctype.h>


char	*mystrtok(char	*str, char *delim)
{
register char	*s1;
register int	quota=0;
static	char	nstr[512];
register char	*s2=nstr;
static	char	*tok_str=0;

    if (str) tok_str=str;
    if ((s1=tok_str) == 0) return (char *)0;
    for(;*s1;s1++)
	if (*s1 != ' ' && *s1 != '\t') break;
    if (*s1 == 0) return (0);
    for(;*s1 != 0;s1++)
    {
	if (quota) {
		if (*s1 == '"') {
			quota = 0;
		} else *s2++ = *s1;
	} else if (*s1 == '"') quota = 1; else
	if (*s1 == '\\' && *(s1+1)) {
	     switch (*++s1) {
		case 'n':
			*s2++ = '\n';
			break;
		case 'r':
			*s2++ = '\r';
			break;
		case 'b':
			*s2++ = 007;
			break;
		case 't':
			*s2++ = '\t';
			break;
		default:
			*s2++ = *s1;
	     }
	} else if (strchr(delim,*s1)) break; else *s2++ = *s1;
    }
    *s2++ = 0;
    if (*s1) s1++;
    tok_str=s1;
    return nstr;
}


#ifdef	unix
/* ========================================================================
 * Put string s in lower case, return s.
 */
char *mystrlwr(char *s)
{
    char *t;
    for (t = s; *t; t++) if (isupper(*t)) *t |= 0x20;
    return s;
}
#endif


LPSTR	mytrim_n( LPSTR ptr, int num_chars )
{
//int  len ;
LPSTR ret;

    if ( num_chars <= 0 )  return ptr;
    ret = ptr + --num_chars;
//    *ret = 0;
    while (*ret == ' ' || *ret == '\t' || *ret == '\000')
	if (ret <= ptr) break; else ret--;

    if (ret != (ptr + num_chars)) *++ret = '\000';
//    *++ret = '\000';
    for(;(int)ptr<(int)ret;ptr++)
	if (*ptr != ' ' && *ptr != '\t') break;
    return ptr;
}


#ifdef	B_ENDIAN
/***********************/
/* Function myintels() */
/***********************/

word	myintels(word v)
{
    /*
     * Convert Intel style 'short' integer to non-Intel non-16-bit
     * host format.  This routine also takes care of byte-ordering.
     */
    byte	*b=(byte *)&v;
    return (word)((b[1] << 8) | b[0]);
}





/***********************/
/* Function myintell() */
/***********************/

dword	myintell(dword v)
{
    /*
     * Convert intel style 'long' variable to non-Intel non-16-bit
     * host format.  This routine also takes care of byte-ordering.
     */
    byte	*sig=(byte *)&v;
    return (((ulg)sig[3]) << 24)
        + (((ulg)sig[2]) << 16)
        + (((ulg)sig[1]) << 8)
        + ((ulg)sig[0]);
}
#endif
