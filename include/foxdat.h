#ifndef __MYFOXDAT__
#define __MYFOXDAT__
#define FIND 1
#define NOFIND 0
#define FAIL    -2
#define SUCCESS 0

#define BLOCKSIZE 256L
#define BLOCKSIZE2 512L
#define CTYPE   'C'
#define NTYPE   'N'
#define LTYPE   'L'
#define DTYPE   'D'
#define MTYPE   'M'
#define UNKOWNTYPE  'U'
#define DELETED	'*'
#define UNDELETED	' '

struct dbf_header_type {
	char attr;/* 03 means no memo fields exist */
		  /* 83 means has memo fields */
	char last_year;
	char last_month;
	char last_day;
	int32 recordnum;
	WORD record_begin_offset;
	WORD record_length;

	char filler[4+16];
};


struct dbf_field_descriptor_type {
	char field_name[11];/* max 10 characters */
	char field_type;/* C ----- Character
			   N ----- Numeric
			   D ----- Date
			   L ----- Logical
			   M ----- Memo
			 */
	int32 field_offset;	 /* don't use it */
	unsigned char field_length;/* max 256 */
	unsigned char point_length;/* nonzero only in Numeric field */
	char reserver[14];
};


#ifdef	notdef
/*  key,point part ------
	{
	char key[key_length];
	reverse long recordno;
	}
*/

  record
	{
	char delete_tag;/* 0x20(' ')  means ok
			   0x2a('*')  means deleted */
	char record[record_length-1];(Numeric are saved in ASCII
				      Logical are one byte 0x20--' '
							   0x46--'F' 
							   0x54--'T'
				      Date are in "19921231" ASCII
				      String are String
				      Memo are in 10 Numric mode ASCII)
	}
	

/* memo field record*/
	{
	char num[10];/* str num */
	}
	this num are block address on filename.DBT
	offset=num*BLOCKSIZE(.eq.256);
	memo field ended with 0x1a (^Z);
	memo file header are 1 block big
	header {
	int file_length;(file length in block unit)
	}

#endif

#endif	/* __MYFOXDAT__ */
