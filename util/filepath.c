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
#ifdef	__WATCOMC__
#include <direct.h>
#endif


char	*mycwd=0;
static	char	my_cwd[MAXPATH];
static	char	pathbuf[MAXPATH];


char	*build_path(char *filep)
{
char		*ss;
	if (*filep == '\\' || *filep == '/') return strcpy(pathbuf, filep);
	if (filep[1] == ':')
		return strcpy(pathbuf, filep);
	if ((ss=getenv(filep)) != 0) {
		return ss;
	}
	if (!mycwd) {
		mycwd = my_cwd;
		mycwd = getcwd(mycwd, MAXPATH);
	}
#ifdef	unix
	sprintf(pathbuf, "%s/%s", mycwd, filep);
#else
	sprintf(pathbuf, "%s\\%s", mycwd, filep);
#endif
	return pathbuf;
}
