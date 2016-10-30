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

#ifndef __FOXINT_H__
#define __FOXINT_H__
#include "foxdat.h"


#ifdef __cplusplus
extern "C" {            /* Assume C declarations for C++ */
#endif  /* __cplusplus */


typedef struct	MYDBF_DATA_tag {
struct	dbf_header_type 		dbf_head;
int					fd;
int					ndbf_fields;
struct	dbf_field_descriptor_type	*dbf_fields;
char					**fields;
int                                     oflag;
} MYDBF_DATA;


int	open_dbf_file(char *fpath, MYDBF_DATA *dbfp, int need_write);
int	creat_dbf_file(char *fpath, MYDBF_DATA *dbfp);
void	close_dbf_file(MYDBF_DATA *dbfp);
int	read_dbf_crecord(MYDBF_DATA *dbf);
int	write_dbf_crecord(MYDBF_DATA *dbf);
int	read_dbf_record(MYDBF_DATA *dbf,long record_no);
int	write_dbf_record(MYDBF_DATA *dbf,long record_no);
int	delete_dbf_record(MYDBF_DATA *dbf,long record_no);
int	append_dbf_record(MYDBF_DATA *dbf);
int	set_dbf_recnum(MYDBF_DATA *dbf, long recnum);
void	goto_dbf_record(MYDBF_DATA *dbf, long recnum);


#ifdef __cplusplus
}                       /* End of extern "C" { */
#endif  /* __cplusplus */

#endif
