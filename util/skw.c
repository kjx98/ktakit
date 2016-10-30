/*********************************************************
 *
 *      String KeyWord Routines
 *
 *	Copyright 1989-2001,2002 Jesse Kuang
 *
 *	$Revision$
 *	$Log$
 *
 *
 *
 *********************************************************
 */
#include "util.h"

#ifndef FALSE
#define	FALSE	(0)
#endif
#ifndef TRUE
#define	TRUE	(-1)
#endif



int
skw_match( char *str, char *kw )
  {
    char*		s;
    char*		k;
    int			required;

    k = kw;

    if( !str )
	return( FALSE );

    strlwr(str);
    while (1)
      {
	s = str;
	required = TRUE;

	while (1)
	  {
	    if( *k == '*' ) required=FALSE, ++k;

	    if( !*s )
	      {
		if( !required || *k=='\0' || *k=='|' )
		    return( TRUE );
		break;
	      }
	    if( *s != *k ) {
		if (!required && (*k == 0 || *k == '|')) return( TRUE );
		break;
	    }

	    ++s; ++k;
	  }
	while (1)
	  {
	    if( !*k )
		return( FALSE );
	    if( *k++ == '|' )
		break;
	  }
      }
  }


int	str_match(char *str, char *kw)
{
register char	*s1,*s2;

    for(s1=str,s2=kw;*s1 && *s2;s1++,s2++)
	if (*s2 == '?') return (str_match(s1+1,s2+1)); else
	if (*s2 == '*')
	{
		s2++;
		while (*s1 && *s2)
			if (str_match(s1++,s2)) return (1);
		return (*s1 == *s2);
	} else if (*s1 != *s2) return (0);
    return (*s1 == *s2);
}
