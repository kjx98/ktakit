#include "ktadefs.h"
#include "util.h"
#include "foxint.h"


#define BUFFERSIZE      1024L
//int	  field_length;
char	dbf_buff[BUFFERSIZE];
#define	start_offset	dbf_head.record_begin_offset


static	void	mystrcpy(char *dst, char *src, int n)
{
    while (*src != 0 && n-- > 0) *dst++ = *src++;
}


#if     defined(WIN32) && !defined(__WATCOMC__)
#include <sys/locking.h>
int	lock(int fd, unsigned off, int len)
{
    lseek(fd, off, SEEK_SET);
    return locking(fd, _LK_LOCK, len);
}


int	unlock(int fd, unsigned off, int len)
{
    lseek(fd, off, SEEK_SET);
    return locking(fd, _LK_UNLCK, len);
}
#endif


static	void	dbf_init_fields(MYDBF_DATA *dbfp)
{
int	i, cnt;
struct	dbf_field_descriptor_type	*dbff;

    cnt = dbfp->ndbf_fields;
    dbfp->fields = malloc(sizeof(char *)*cnt);
    if (dbfp->fields == 0) {
	warn("No memory for DBF fields");
	return;
    }
    memset(dbfp->fields, 0, sizeof(char *)*cnt);
    for(i=0,dbff=dbfp->dbf_fields;i<cnt;i++,dbff++)
	if ((dbfp->fields[i]=malloc(dbff->field_length+4)) == 0) {
		warn("No memory for DBF fields buf");
		while(--i>=0) free(dbfp->fields[i]);
		free(dbfp->fields);
		dbfp->fields = 0;
		return;
	}
}


// fields --> dbf_buff
static	void	build_dbf_buffer(MYDBF_DATA *dbfp)
{
int		i;
struct	dbf_field_descriptor_type	*dbff;
    memset(dbf_buff, ' ', dbfp->dbf_head.record_length);
    dbf_buff[0] = UNDELETED;
    for(i=0,dbff=dbfp->dbf_fields;i<dbfp->ndbf_fields;i++,dbff++) {
	if (dbff->field_type == 'N') {
		int		len;
		len = strlen(dbfp->fields[i]);
		if (len < dbff->field_length)
			memcpy(dbf_buff + dbff->field_offset + (dbff->field_length-len),
				dbfp->fields[i], len); else
			memcpy(dbf_buff + dbff->field_offset, dbfp->fields[i],
				dbff->field_length);
	} else mystrcpy(dbf_buff + dbff->field_offset, dbfp->fields[i], dbff->field_length);
    }
}


// dbf_buff --> fields
static	void	split_dbf_buffer(MYDBF_DATA *dbfp)
{
int		i;
struct	dbf_field_descriptor_type	*dbff;
    for(i=0,dbff=dbfp->dbf_fields;i<dbfp->ndbf_fields;i++,dbff++) {
	memcpy(dbfp->fields[i], dbf_buff + dbff->field_offset, dbff->field_length);
	*(dbfp->fields[i] + dbff->field_length) = 0;
    }
}


int	open_dbf_file(char *fpath, MYDBF_DATA *dbfp, int need_write)
{
int			fd;
int			i, cnt;
struct	dbf_header_type 	dbf_header;

#ifdef	unix
    fd = open(fpath, (need_write)?O_RDWR:O_RDONLY);
#else
    fd = sopen(fpath, ((need_write)?O_RDWR:O_RDONLY) | O_BINARY, SH_DENYNO);
#endif
    if (fd < 0) return 0;
    if (read(fd,&dbf_header,sizeof(dbf_header)) < sizeof(dbf_header)) {
	close(fd);
	return 0;
    }
#ifdef	B_ENDIAN
    dbf_header.record_length = myintels(dbf_header.record_length);
    dbf_header.record_begin_offset = myintels(dbf_header.record_begin_offset);
    dbf_header.recordnum = myintell(dbf_header.recordnum);
#endif
    if (dbfp->fields == 0) {
	struct	dbf_field_descriptor_type	*dbff;
	struct	dbf_field_descriptor_type	dbfs;

	if (dbf_header.record_length > BUFFERSIZE) {
		close(fd);
		return 0;
	}

	cnt = (dbf_header.record_begin_offset - sizeof(dbfp->dbf_head)) /
		sizeof(struct dbf_field_descriptor_type);
	if (cnt != dbfp->ndbf_fields) {
		close(fd);
		warn("Invalid DBF file(fields number diff) %s", fpath);
		return 0;
	}
	for(i=0;i<cnt;i++) {
		if (read(fd, &dbfs, sizeof(dbfs)) != sizeof(dbfs)) {
			close(fd);
			warn("Malform DBF file %s", fpath);
			return 0;
		}
		if (dbfs.field_length != dbfp->dbf_fields[i].field_length) {
			close(fd);
			warn("DBF %s structure changed", fpath);
			return 0;
		}
	}
	for(i=0,cnt=1,dbff=dbfp->dbf_fields;i<dbfp->ndbf_fields;i++, dbff++) {
		dbff->field_offset = cnt;
		cnt += dbff->field_length;
	}
	dbf_init_fields(dbfp);
	if (dbfp->fields == 0) {
		close(fd);
		return 0;
	}
    }
    dbfp->dbf_head.record_length = dbf_header.record_length;
    dbfp->dbf_head.recordnum = dbf_header.recordnum;
    dbfp->dbf_head.record_begin_offset = dbf_header.record_begin_offset;
    lseek(fd, dbfp->dbf_head.record_begin_offset, SEEK_SET);
    dbfp->fd = fd;
    dbfp->oflag = TRUE;
    return TRUE;
}


int	creat_dbf_file(char *fpath, MYDBF_DATA *dbfp)
{
int	fd;
int	i, cnt;
struct	dbf_field_descriptor_type	*dbff;
time_t	cur_dtime=time(0);
struct	tm	*tmp=localtime(&cur_dtime);
int	rec_len;
#ifdef	B_ENDIAN
struct	dbf_header_type 	dbf_header;
#endif

#ifdef  unix
    fd = creat(fpath, 0644);
#else
    fd = open(fpath, O_BINARY | O_CREAT | O_RDWR, S_IREAD | S_IWRITE);
#endif
    if (fd < 0) return 0;

    dbfp->dbf_head.last_year = tmp->tm_year;
    dbfp->dbf_head.last_month = tmp->tm_mon+1;
    dbfp->dbf_head.last_day = tmp->tm_mday;
    dbfp->dbf_head.recordnum = 0;
    cnt = dbfp->ndbf_fields * sizeof(struct dbf_field_descriptor_type);
    dbfp->dbf_head.record_begin_offset = cnt + sizeof(dbfp->dbf_head) + 1;
    for(i=0,rec_len=1,dbff=dbfp->dbf_fields;i<dbfp->ndbf_fields;i++,dbff++) {
	dbff->field_offset = 0;
	rec_len += dbff->field_length;
    }
    dbfp->dbf_head.record_length = rec_len;
#ifdef	B_ENDIAN
    memcpy(&dbf_header, dbfp->dbf_head, sizeof(dbf_header));
    dbf_header.record_length = myintels(dbf_header.record_length);
    dbf_header.record_begin_offset = myintels(dbf_header.record_begin_offset);
    dbf_header.recordnum = myintell(dbf_header.recordnum);
    if (write(fd,&dbf_header,sizeof(dbf_header)) < sizeof(dbf_header)) {
	close(fd);
	return 0;
    }
#else
    if (write(fd,&dbfp->dbf_head,sizeof(dbfp->dbf_head)) < sizeof(dbfp->dbf_head)) {
	close(fd);
	return 0;
    }
#endif
    if (write(fd, dbfp->dbf_fields, cnt) != cnt) {
	close(fd);
	return 0;
    }
    write(fd, "\r", 1);
    for(i=0,cnt=1,dbff=dbfp->dbf_fields;i<dbfp->ndbf_fields;i++, dbff++) {
	dbff->field_offset = cnt;
	cnt += dbff->field_length;
    }
    dbfp->dbf_head.record_length = cnt;
    if (dbfp->fields == 0) {
	dbf_init_fields(dbfp);
	if (dbfp->fields == 0) {
		close(fd);
		return 0;
	}
    }
    dbfp->fd = fd;
    dbfp->oflag = TRUE;
    return TRUE;
}


void	close_dbf_file(MYDBF_DATA *dbfp)
{
int	i;
    if (dbfp->oflag == 0) {
	warn("Close closed dbf");
	return;
    }
    close(dbfp->fd);
    dbfp->fd = -1;
    if (dbfp->fields != 0) {
	for(i=0;i<dbfp->ndbf_fields;i++) free(dbfp->fields[i]);
	free(dbfp->fields);
	dbfp->fields = 0;
    }
    dbfp->oflag = 0;
}


int	read_dbf_crecord(MYDBF_DATA *dbf)
{

    if	(dbf->fd < 0) return -1;

    if (read(dbf->fd,dbf_buff,dbf->dbf_head.record_length) < dbf->dbf_head.record_length) return -1;
    if (dbf_buff[0] == DELETED) return 0;
    // form fields
    split_dbf_buffer(dbf);

    return 1;
}



int	write_dbf_crecord(MYDBF_DATA *dbf)
{

    if	(dbf->fd < 0) return 0;

    // form dbf_buff
    build_dbf_buffer(dbf);
    if (write(dbf->fd, dbf_buff, dbf->dbf_head.record_length) < dbf->dbf_head.record_length) return 0;
    return TRUE;
}


int	read_dbf_record(MYDBF_DATA *dbf,long record_no)
{
long	offset;

#ifdef	STRONG_VALID
    if	(dbf->fd < 0 || record_no <= 0) return -1;
#else
    if	(dbf->fd < 0) return -1;
#endif

    offset=dbf->start_offset+ (long)(record_no-1)*(long)dbf->dbf_head.record_length;
    lseek(dbf->fd,offset,SEEK_SET);
    if (read(dbf->fd,dbf_buff,dbf->dbf_head.record_length) < dbf->dbf_head.record_length) return -1;
    if (dbf_buff[0] == DELETED) return 0;
    // form fields
    split_dbf_buffer(dbf);

    return 1;
}



int	write_dbf_record(MYDBF_DATA *dbf,long record_no)
{
long	offset;
int	ret=TRUE;

#ifdef	STRONG_VALID
    if	(dbf->fd < 0 || record_no <= 0) return 0;
#else
    if	(dbf->fd < 0) return 0;
#endif

    offset=dbf->start_offset+ (long)(record_no-1)*(long)dbf->dbf_head.record_length;

#ifndef unix
    if (lock(dbf->fd,offset,dbf->dbf_head.record_length))
	return 0;
#endif

    // form dbf_buff
    build_dbf_buffer(dbf);
    lseek(dbf->fd, offset, SEEK_SET);
    if (write(dbf->fd, dbf_buff, dbf->dbf_head.record_length) < dbf->dbf_head.record_length) ret=0;
#ifndef unix
    unlock(dbf->fd, offset, dbf->dbf_head.record_length);
#endif
    return ret;
}


int	delete_dbf_record(MYDBF_DATA *dbf,long record_no)
{
long	offset;

#ifdef	STRONG_VALID
    if	(dbf->fd < 0 || record_no <= 0) return 0;
#else
    if	(dbf->fd < 0) return 0;
#endif
    dbf_buff[0] = DELETED;

    offset=dbf->start_offset+ (long)(record_no-1)*(long)dbf->dbf_head.record_length;

#ifndef unix
    if (lock(dbf->fd,offset,dbf->dbf_head.record_length))
	return 0;
#endif

    lseek(dbf->fd, offset, SEEK_SET);
    write(dbf->fd, dbf_buff, 1);
#ifndef unix
    unlock(dbf->fd, offset, dbf->dbf_head.record_length);
#endif
    return TRUE;
}


int	append_dbf_record(MYDBF_DATA *dbf)
{
long	record_no;
int	ret;
int	fd;
struct	dbf_header_type dbf_header;

#ifdef	STRONG_VALID
    if	(dbf->fd < 0 || record_no <= 0) return 0;
#else
    if	(dbf->fd < 0) return 0;
#endif
    fd = dbf->fd;

#ifndef unix
    if (lock(fd, 0, sizeof(dbf_header))) return 0;
#endif

    lseek(fd, 0, SEEK_SET);
    if (read(fd,&dbf_header,sizeof(dbf_header)) < sizeof(dbf_header)) {
#ifndef unix
	unlock(fd, 0, sizeof(dbf_header));
#endif
	return 0;
    }
    dbf->dbf_head.recordnum = myintell(dbf_header.recordnum);
    record_no = ++(dbf->dbf_head.recordnum);
    ret =  write_dbf_record(dbf,record_no);
    if (!ret) {
#ifndef unix
	unlock(fd, 0, sizeof(dbf_header));
#endif
	return 0;
    }

    dbf_header.recordnum = myintell(dbf->dbf_head.recordnum);
    lseek(fd, 0, SEEK_SET);
    if (write(fd, &dbf_header, sizeof(dbf_header)) < sizeof(dbf_header))
	ret = 0;
    else
	ret = TRUE;

#ifndef unix
    unlock(fd, 0, sizeof(dbf_header));
#endif
    return ret;
}


int	set_dbf_recnum(MYDBF_DATA *dbf, long recnum)
{
int	ret;
int	fd;
struct	dbf_header_type dbf_header;

#ifdef	STRONG_VALID
    if	(dbf->fd < 0 || recnum <= 0) return 0;
#else
    if	(dbf->fd < 0) return 0;
#endif
    fd = dbf->fd;

#if	!defined(unix) // && !defined(WIN32)
    if (lock(fd, 0, sizeof(dbf_header))) return 0;
#endif

    lseek(fd, 0, SEEK_SET);
    if (read(fd,&dbf_header,sizeof(dbf_header)) < sizeof(dbf_header)) {
#if	!defined(unix) // && !defined(WIN32)
	unlock(fd, 0, sizeof(dbf_header));
#endif
	return 0;
    }

    dbf_header.recordnum = myintell(recnum);
    lseek(fd, 0, SEEK_SET);
    if (write(fd, &dbf_header, sizeof(dbf_header)) < sizeof(dbf_header))
	ret = 0;
    else
	ret = TRUE;

#if	!defined(unix) // && !defined(WIN32)
    unlock(fd, 0, sizeof(dbf_header));
#endif
    return ret;
}


void	goto_dbf_record(MYDBF_DATA *dbf, long record_no)
{
long	offset;

#ifdef	STRONG_VALID
    if	(dbf->fd < 0 || record_no <= 0) return 0;
#else
    if	(dbf->fd < 0) return;
#endif
    offset=dbf->start_offset+ (long)(record_no-1)*(long)dbf->dbf_head.record_length;
    lseek(dbf->fd,offset,SEEK_SET);
}
