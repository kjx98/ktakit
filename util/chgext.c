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
#include "util.h"

static	char	npath[MAXPATH];


char *chgext(const char *path, const char *newext)
{
    return chgext_b(path, newext, npath);
}


char *chgext_b(const char *path, const char *newext, char *newpath)
{
char	*p;
int	len;

#ifdef	unix
    /* Convert to Unix-style path name */

    if (path == newpath) {
	for (p = path,len=0; *p; ++p,len++)
		if ('\\' == *p) newpath[len] = '/';
    } else {
	for (p = path,len=0; *p; ++p,len++)
		if ('\\' == *p) newpath[len] = '/'; else newpath[len] = *p;
    }

    newpath[len] = 0;

    /* Find extension or point to end for appending */

    if (NULL == (p = strrchr(newpath, '.')) || NULL != strchr(p, '/'))
	p = strcpy(&newpath[len], ".");
#else
    /* Convert to DOS-style path name */

    if (path == newpath) len = strlen(path); else
    {
	for (p = path,len=0; *p; ++p,len++)
		if (path != newpath) newpath[len] = *p;
    }

    newpath[len] = 0;

    /* Find extension or point to end for appending */

    if (NULL == (p = strrchr(newpath, '.')) || NULL != strchr(p, '\\'))
	p = strcpy(&newpath[len], ".");
#endif
    ++p;

    /* Add new extension */

    while ('.' == *newext) ++newext;
    p[3] = 0;
    strncpy(p, newext, 3);
    return newpath;
}
