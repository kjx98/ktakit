
/*  INI 1.0  */
/*  Copyright (c) 1986-87 Fran Finnegan Associates  */
/*  Copyright (c) 1988-92 Finnegan O'Malley & Company Inc.  */
/*  All Rights Reserved.  */
/*  First Published in PC Magazine, June 30, 1992.  */

/*  originally developed under Microsoft C 4.0  */
/*  updated for Microsoft C 5.1  */

#include "ktadefs.h"

#ifndef __MINGW32__
static char * strupr(char *str);
#endif
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


#ifndef TRUE
#define TRUE	1
#endif
#ifndef FALSE
#define FALSE	0
#endif

#define YES	1
#define NO	0

#define ON	1
#define OFF	0

#define SET	1
#define CLEAR	0

#define BREAK	0

#define OK	0
#define ERR	(-1)

#define K	<<10

#define Null	'\0'
#define NUL	0L

#if	defined(M_I86MM)
#define NULC	0L
#elif	defined(M_I86CM)
#define NULC	0
#else
#define NULC	NULL
#endif

#define and	&&
#define or	||
#define no	!
#define not	!
#define cannot	!


/*  a, b, p, q & r should be of "char [{near|far}] *" form  */

    /*  the following are boolean in nature  */

#define IsStrNULL(p)    (!(p))      /*  NULL?  */
#define IsStrNull(p)    (!*(p))     /*  Null ("" or "\0")?  */

#define IsStrEqual(a,b) (!strcmp(a,b))  /*  equal?  */
#define IsStrEquiv(a,b) (!strcasecmp(a,b)) /*  equivalent?  */

#define IsStrOnly   !StrVerify  /*  verifies chars are in string  */

    /*  the following are of "p = StrFunction(q, r)" form  */

#define StrNull(q)	strchr(q,'\0')
            /*  find string-end Null;  p -> null terminator  */

#define StrCrop(q,r)	(StrTrunc(q,r),StrLeft(q,r))
            /*  crop both ends, copy;  p==q:  first non-"r"-char  */

#define StrLeft(q,r)	strcpy(q,StrSkip(q,r))
            /*  crop left side, copy;  p==q:  first non-"r"-char  */
#define StrLeftTo(q,r)	strcpy(q,StrSkipTo(q,r))
            /*  crop left side, copy;  p==q:  first "r"-char  */

#define StrSkip(q,r)	(&(q)[strspn(q,r)])
            /*  skip chars in "r";     p -> first non-"r"-char  */
#define StrSkipTo(q,r)	(&(q)[strcspn(q,r)])
            /*  skip to char in "r";   p -> first "r"-char  */

#define StrPlural(n,s,p) ((n)==1?(s):(p))
            /*  if singular: "", "y"; if plural: "s", "es", "ies"  */

    /*  BEWARE OF SIDE EFFECTS IN THE FOLLOWING  */

#define SubStrCL(to,from,col,len)	SubStr(to,from,(col)-1,len)
#define SubStrCC(to,from,col1,col2)	SubStr(to,from,(col1)-1,(col2)-(col1)+1)
#define SubStrOO(to,from,off1,off2)	SubStr(to,from,off1,(off2)-(off1)+1)
#define SubStrOL            SubStr  /*  (to,from,off,len)  */

    /*  the following are of "p = StrFunction(q, r)" form  */

#define TABSTOPS    8   /*  columns per tab in StrDetab and StrEntab  */

extern  char    *StrDetab(char *);      /*  remove tabs, expanding  */
extern  char    *StrEntab(char *);      /*  insert tabs, contracting  */
extern  char    *StrIndex(char *, char *);  /*  same as MS-C "strstr"  */
extern  char    *StrTrunc(char *, char *);  /*  truncates chars from end  */
extern  char    *StrUcLc(char *);       /*  makes upper/lower case  */
extern  char    *StrVerify(char *, char *); /*  verifies chars are in string  */
extern  char    *SubStr(char *, char *, int, int);  /*  PL/I "substr"  */


	unsigned char	CharASCII[] = { '\000',
					'\001',
					'\002',
					'\003',
					'\004',
					'\005',
					'\006',
					'\007',
					'\010',
					'\011',
					'\012',
					'\013',
					'\014',
					'\015',
					'\016',
					'\017',
					'\020',
					'\021',
					'\022',
					'\023',
					'\024',
					'\025',
					'\026',
					'\027',
					'\030',
					'\031',
					'\032',
					'\033',
					'\034',
					'\035',
					'\036',
					'\037',
					'\040',
					'\041',
					'\042',
					'\043',
					'\044',
					'\045',
					'\046',
					'\047',
					'\050',
					'\051',
					'\052',
					'\053',
					'\054',
					'\055',
					'\056',
					'\057',
					'\060',
					'\061',
					'\062',
					'\063',
					'\064',
					'\065',
					'\066',
					'\067',
					'\070',
					'\071',
					'\072',
					'\073',
					'\074',
					'\075',
					'\076',
					'\077',
					'\100',
					'\101',
					'\102',
					'\103',
					'\104',
					'\105',
					'\106',
					'\107',
					'\110',
					'\111',
					'\112',
					'\113',
					'\114',
					'\115',
					'\116',
					'\117',
					'\120',
					'\121',
					'\122',
					'\123',
					'\124',
					'\125',
					'\126',
					'\127',
					'\130',
					'\131',
					'\132',
					'\133',
					'\134',
					'\135',
					'\136',
					'\137',
					'\140',
					'\141',
					'\142',
					'\143',
					'\144',
					'\145',
					'\146',
					'\147',
					'\150',
					'\151',
					'\152',
					'\153',
					'\154',
					'\155',
					'\156',
					'\157',
					'\160',
					'\161',
					'\162',
					'\163',
					'\164',
					'\165',
					'\166',
					'\167',
					'\170',
					'\171',
					'\172',
					'\173',
					'\174',
					'\175',
					'\176',
					'\177',
			 (unsigned char)'\200',
			 (unsigned char)'\201',
			 (unsigned char)'\202',
			 (unsigned char)'\203',
			 (unsigned char)'\204',
			 (unsigned char)'\205',
			 (unsigned char)'\206',
			 (unsigned char)'\207',
			 (unsigned char)'\210',
			 (unsigned char)'\211',
			 (unsigned char)'\212',
			 (unsigned char)'\213',
			 (unsigned char)'\214',
			 (unsigned char)'\215',
			 (unsigned char)'\216',
			 (unsigned char)'\217',
			 (unsigned char)'\220',
			 (unsigned char)'\221',
			 (unsigned char)'\222',
			 (unsigned char)'\223',
			 (unsigned char)'\224',
			 (unsigned char)'\225',
			 (unsigned char)'\226',
			 (unsigned char)'\227',
			 (unsigned char)'\230',
			 (unsigned char)'\231',
			 (unsigned char)'\232',
			 (unsigned char)'\233',
			 (unsigned char)'\234',
			 (unsigned char)'\235',
			 (unsigned char)'\236',
			 (unsigned char)'\237',
			 (unsigned char)'\240',
			 (unsigned char)'\241',
			 (unsigned char)'\242',
			 (unsigned char)'\243',
			 (unsigned char)'\244',
			 (unsigned char)'\245',
			 (unsigned char)'\246',
			 (unsigned char)'\247',
			 (unsigned char)'\250',
			 (unsigned char)'\251',
			 (unsigned char)'\252',
			 (unsigned char)'\253',
			 (unsigned char)'\254',
			 (unsigned char)'\255',
			 (unsigned char)'\256',
			 (unsigned char)'\257',
			 (unsigned char)'\260',
			 (unsigned char)'\261',
			 (unsigned char)'\262',
			 (unsigned char)'\263',
			 (unsigned char)'\264',
			 (unsigned char)'\265',
			 (unsigned char)'\266',
			 (unsigned char)'\267',
			 (unsigned char)'\270',
			 (unsigned char)'\271',
			 (unsigned char)'\272',
			 (unsigned char)'\273',
			 (unsigned char)'\274',
			 (unsigned char)'\275',
			 (unsigned char)'\276',
			 (unsigned char)'\277',
			 (unsigned char)'\300',
			 (unsigned char)'\301',
			 (unsigned char)'\302',
			 (unsigned char)'\303',
			 (unsigned char)'\304',
			 (unsigned char)'\305',
			 (unsigned char)'\306',
			 (unsigned char)'\307',
			 (unsigned char)'\310',
			 (unsigned char)'\311',
			 (unsigned char)'\312',
			 (unsigned char)'\313',
			 (unsigned char)'\314',
			 (unsigned char)'\315',
			 (unsigned char)'\316',
			 (unsigned char)'\317',
			 (unsigned char)'\320',
			 (unsigned char)'\321',
			 (unsigned char)'\322',
			 (unsigned char)'\323',
			 (unsigned char)'\324',
			 (unsigned char)'\325',
			 (unsigned char)'\326',
			 (unsigned char)'\327',
			 (unsigned char)'\330',
			 (unsigned char)'\331',
			 (unsigned char)'\332',
			 (unsigned char)'\333',
			 (unsigned char)'\334',
			 (unsigned char)'\335',
			 (unsigned char)'\336',
			 (unsigned char)'\337',
			 (unsigned char)'\340',
			 (unsigned char)'\341',
			 (unsigned char)'\342',
			 (unsigned char)'\343',
			 (unsigned char)'\344',
			 (unsigned char)'\345',
			 (unsigned char)'\346',
			 (unsigned char)'\347',
			 (unsigned char)'\350',
			 (unsigned char)'\351',
			 (unsigned char)'\352',
			 (unsigned char)'\353',
			 (unsigned char)'\354',
			 (unsigned char)'\355',
			 (unsigned char)'\356',
			 (unsigned char)'\357',
			 (unsigned char)'\360',
			 (unsigned char)'\361',
			 (unsigned char)'\362',
			 (unsigned char)'\363',
			 (unsigned char)'\364',
			 (unsigned char)'\365',
			 (unsigned char)'\366',
			 (unsigned char)'\367',
			 (unsigned char)'\370',
			 (unsigned char)'\371',
			 (unsigned char)'\372',
			 (unsigned char)'\373',
			 (unsigned char)'\374',
			 (unsigned char)'\375',
			 (unsigned char)'\376',
			 (unsigned char)'\377'};


#define A_EOF   '\032'  /* 0x1A ^Z end-of-file          */

#define A_NUL   '\000'  /* 0x00 ^@ null                 */
#define A_SOH   '\001'  /* 0x01 ^A start of heading     */
#define A_STX   '\002'  /* 0x02 ^B start of text        */
#define A_ETX   '\003'  /* 0x03 ^C end of text          */
#define A_EOT   '\004'  /* 0x04 ^D end of transmission  */
#define A_ENQ   '\005'  /* 0x05 ^E enquiry              */
#define A_ACK   '\006'  /* 0x06 ^F acknowledge          */
#define A_BEL   '\007'  /* 0x07 ^G bell              \a */
#define A_BS    '\010'  /* 0x08 ^H backspace         \b */
#define A_HT    '\011'  /* 0x09 ^I horizontal tab    \t */
#define A_LF    '\012'  /* 0x0A ^J line feed         \n */
#define A_VT    '\013'  /* 0x0B ^K vertical tab      \v */
#define A_FF    '\014'  /* 0x0C ^L form feed         \f */
#define A_CR    '\015'  /* 0x0D ^M carriage return   \r */
#define A_SO    '\016'  /* 0x0E ^N shift out            */
#define A_SI    '\017'  /* 0x0F ^O shift in             */
#define A_DLE   '\020'  /* 0x10 ^P data link escape     */
#define A_DC1   '\021'  /* 0x11 ^Q device control: xon  */
#define A_DC2   '\022'  /* 0x12 ^R device control: 2    */
#define A_DC3   '\023'  /* 0x13 ^S device control: xoff */
#define A_DC4   '\024'  /* 0x14 ^T device control: 4    */
#define A_NAK   '\025'  /* 0x15 ^U negative acknowledge */
#define A_SYN   '\026'  /* 0x16 ^V synchronous idle     */
#define A_ETB   '\027'  /* 0x17 ^W end of trans. block  */
#define A_CAN   '\030'  /* 0x18 ^X cancel (void data)   */
#define A_EM    '\031'  /* 0x19 ^Y end of medium        */
#define A_SUB   '\032'  /* 0x1A ^Z substitute           */
#define A_ESC   '\033'  /* 0x1B ^[ escape               */
#define A_FS    '\034'  /* 0x1C ^\ file separator   EOF */
#define A_GS    '\035'  /* 0x1D ^] group separator  EOG */
#define A_RS    '\036'  /* 0x1E ^^ record separator EOR */
#define A_US    '\037'  /* 0x1F ^_ unit/field separator */

#define A_DEL   '\177'  /* 0x7F    delete               */


/*
*   All output pointers to char arrays that are to be "filled in"
*   must have their memory allocated per the #defines herein.
*/

#define exist(sz)   (!access(sz, 00))   /* #include <io.h> */

                /* input            output       */
#define FILESPEC    64  /* "d:\\path\\filename.ext"     same         */
#define FILESPEC_DRIVE  3   /* "d:", "d" or ""              "d:" or ""   */
#define FILESPEC_PATH   63  /* "\\path\\", ..., "\\" or ""  same         */
#define FILESPEC_NAME   9   /* "filename"                   same         */
#define FILESPEC_EXT    5   /* ".ext", "ext" or ""          ".ext" or "" */

struct  CHKDSK {    /* Do NOT change the order of structure elements! */
    unsigned int    uiSectorsPerCluster;    /* A */
    unsigned int    uiAvailableClusters;    /* B */
    unsigned int    uiBytesPerSector;   /* C */
    unsigned int    uiTotalClusters;    /* D */
    long        lTotalDiskSpace;    /* C * A * D */
    long        lAvailableOnDisk;   /* C * A * B */
    unsigned int    uiBytesPerCluster;  /* C * A */
    unsigned int    uiPercentFreeSpace; /* (B * 100) / D and rounded */
    };

extern  char        *MakeFilespec(char *, char *, char *, char *, char *);
extern  void        ParseFilespec(char *, char *, char *, char *, char *);

extern  void        chdrv(char *);
extern  struct CHKDSK   chkdsk(char *);
extern  void        verify(int);
extern  int     IsPrnOn(void);  /* M model only */



/**
* module	DBC\CONFIG
*
* purpose	Used to access "CONFIG.SYS-format" and "WIN.INI-format" files.
*		Similar to Windows Initialization File functions.
*		The following file format is processed:
*
*			; comment
*			[SectionName]	or  [SectionName] ; comment
*			KeyName=string	or  KeyName=string ; comment
*			KeyName=	or  KeyName= ; null string
*			       =string	or	   =string ; blank KeyName
*			KeyName 	or  KeyName ; same as KeyName=
*
*		Comments must be on separate lines or else the ';' must be
*		preceded by a blank (to allow ';' to occur in strings).
*		Blanks are not allowed between "SectionName" and the brackets.
*		"SectionName" and "KeyName" are not case-dependent, so the
*		strings may contain any combination of upper and lower case.
*		If there is no equal sign, the entire line is assumed the key.
*		The existence of an equal sign (after the removal of comments)
*		determines that a string exists; otherwise it is assumed Null.
*		Blanks around "KeyName" and "string" are ignored.  A null or
*		blank KeyName (when an equal sign is present) is assumed to be
*		a single blank.  Blank lines are allowed anywhere and ignored.
*		SectionNames must be unique, since only the first one is used.
*		KeyNames should be unique, since all are processed similarly.
*
* logic 	The file is processed sequentially.  Each line is read, detabbed
*		and parsed and then a "call-back" function is called with the
*		resulting line components determined.  The components are:
*
*			FilePre 		file line (including comments)
*
*		and (if available)
*
*			FileKey -> FilePost	left of the equal sign
*						(a blank if there is no key)
*		and	FileString -> FilePost	right of the equal sign
*						(Null if there is no string)
*
*		The call-back function returns a code to indicate continued
*		scanning.  The parser returns if either the end-of-file is
*		reached or the call-back function ended the parsing.
*
* cautions	Argument checking is not performed.
*
* includes	<dbc/config.h>
*
* copyright	(c) Fran Finnegan Associates 1986, 1987
* copyright	(c) 1988-92 Finnegan O'Malley & Company Inc.
**/

#ifndef LINT_ARGS
#define LINT_ARGS
#endif

#define NAME_SIZE	64	/* for FileNames, SectionNames and KeyNames */
#define LINE_SIZE	256	/* for file lines */

#define ABOUT_TO_BEGIN	-1	/* to call-back:  about to begin reading file */
#define NOT_IN_SECTION	NO	/* to call-back:  not in Section requested */
#define ARE_IN_SECTION	YES	/* to call-back:  are in Section requested */
#define END_OF_PROCESS	2	/* to call-back:  end-of-process was reached */

#define CONTINUE	1	/* to ParseFile:  continue reading file */
#define STOP		2	/* to ParseFile:  stop reading file */

#define ACCESS_EXISTS	0	/* "access" existence-only mode */
#define ACCESS_UNLINK	6	/* "access" read-and-write mode */

	int		gbConfigCloseFileOnExit = NO;
	int		gbConfigDeleteBakOnExit = YES;

static	int		mfnGetConfigString(int);
static	int		mfnWriteConfigString(int);
static	int		mfnGetConfigSection(int);
static	int		mfnGetConfigKeys(int);

static	void		CloseFile(void);
static	void		ParseFile(char *, char *, int (*)(int));
static	void		ParseSection(char *, char *);
static	void		ParseKey(char *, char *);
static	char		*MemoryGetString(char *, int, char **);

static	char		*mlpmemConfig;	    /* memory file in process */
static	FILE		*mpfileConfig;		/* stream file in process */
static	char		*mpszUserDefault;	/* user supplied */
static	char		*mpszUserReturned;	/* user supplied */
static	unsigned int	  muiUserSize;		/* user supplied */
static	char		*mpszUserString;	/* user supplied */
static	char		*mpszFileKey;		/* pointer to mszFilePost */
static	char		*mpszFileString;	/* pointer to mszFilePost */
static	int		   miReturn;		/* return code */
static	int		   mbWriteRequest;	/* WriteConfig... request */
static	char	mszUserFile   [NAME_SIZE],	/* "d:\\path\\filename.ext" */
		mszUserSection[NAME_SIZE],	/* "[SectionName]" or "" */
		mszUserKey    [NAME_SIZE],	/* "KeyName" or " " */
		mszFilePre    [LINE_SIZE],	/* pre-parsing line */
		mszFilePost   [LINE_SIZE];	/* post-parsing line */

/*p*/

/**
* function	GetConfigInt
*
* purpose	Similar to Windows "GetProfileInt".
*
* logic 	Calls "GetConfigString".
*
* cautions	Unless the file contains a valid int, the default is returned.
*
* usage 	iKeyValue = GetConfigInt(pszFilespec, pszSection, pszKey,
*			iDefault);
*
* arguments	pszFilespec	"d:\\path\\filename.ext" format.
*				pszFilespec must not be NULL.
*		pszSection	"SectionName" of "[SectionName]".
*				pszSection may be NULL, implying "".
*		pszKey		"KeyName" of "KeyName=string".
*				pszKey may be NULL, implying " ".
*		iDefault	Default int if the SectionName/KeyName
*				combination does not exist or is Null.
*
* returns	iKeyValue = int value of the key string.
*
* modifies	no externals.
*
* examples	iDOSbuffers = GetConfigInt("C:\\CONFIG.SYS", NULL, "buffers",
*			-1);
*
* copyright	(c) Fran Finnegan Associates 1986, 1987
**/

extern	int		GetMemoryInt(char *lpmemConfig, char *pszSection, char *pszKey, int iDefault)
{
auto	int		iReturn;

CloseFile();
mlpmemConfig = lpmemConfig;
iReturn = GetConfigInt(NULL, pszSection, pszKey, iDefault);
mlpmemConfig = (char *)NULL;
return (iReturn);
}


extern  int             GetConfigInt(char *pszFilespec, char *pszSection, char *pszKey, int iDefault)
{
auto	char		szReturned[12]; 	/* [12] should be sufficient */

return ((GetConfigString(pszFilespec, pszSection, pszKey, "", szReturned,
		sizeof(szReturned)) and
	strspn(szReturned, "+-0123456789"))?  atoi(szReturned):  iDefault);
}

/*p*/

/**
* function	GetConfigString
*
* purpose	Similar to Windows "GetProfileString".
*
* logic 	See "arguments" below.
*
* cautions	The "Returned" buffer must be at least 2 bytes long.  It should
*		be one byte longer than the longest expected "string".
*
* usage 	iLength = GetConfigString(pszFilespec, pszSection, pszKey,
*			pszDefault, pszReturned, uisize);
*
* arguments	pszFilespec	"d:\\path\\filename.ext" format.
*				pszFilespec must not be NULL.
*		pszSection	"SectionName" of "[SectionName]".
*				pszSection may be NULL, implying "".
*				Blanks around pszSection are ignored.
*				Brackets are not included, but added herein.
*				If pszSection == "":
*					SectionNames in the file are ignored.
*		pszKey		"KeyName" of "KeyName=string".
*				pszKey may be NULL, implying " ".
*				Blanks around pszKey are ignored.
*				If pszKey == "":
*					A null or blank KeyName is searched for.
*		pszDefault	Default string if the SectionName/KeyName
*				combination does not exist or is Null.
*				If pszDefault == NULL:
*					The KeyName itself is the default.
*				If pszDefault == "":
*					The Null string is the default.
*				The default is used if either the KeyName does
*				not exist or the Key string exists but is Null.
*		pszReturned	Returned-string buffer (at least 2 bytes).
*		uisize		Returned-string buffer size.
*
* returns	iLength = int length of returned string.  0 indicates Null ("").
*
* modifies	no externals.
*
* examples	auto	char	szDOSshell[64];
*
*		iDOSshell = GetConfigString("C:\\CONFIG.SYS", NULL, "shell",
*			"C:\\COMMAND.COM", szDOSshell, sizeof(szDOSshell));
*
* copyright	(c) Fran Finnegan Associates 1986, 1987
**/

extern	int		GetMemoryString(char *lpmemConfig, char *pszSection, char *pszKey,
				char *pszDefault, char *pszReturned, unsigned uisize)
{
auto	int		iReturn;

CloseFile();
mlpmemConfig = lpmemConfig;
iReturn = GetConfigString(NULL, pszSection, pszKey, pszDefault, pszReturned,
	uisize);
mlpmemConfig = (char *)NULL;
return (iReturn);
}

extern	int		GetConfigString(char *pszFilespec, char *pszSection, char *pszKey,
				char *pszDefault, char *pszReturned, unsigned uisize)
{
ParseKey(mszUserKey, pszKey);					/* Key	    */
mpszUserDefault = pszDefault;					/* Default  */
mpszUserReturned = pszReturned; 				/* Returned */
muiUserSize = uisize;						/* Size     */
ParseFile(pszFilespec, pszSection, mfnGetConfigString);
return (miReturn);
}

/*p*/

static	int		mfnGetConfigString(iCase)

	int		iCase;
{
static	FILE		*pfileConfig;		/* stream file last processed */
static	int		bInSection = NO,
#if	!defined(__TURBOC__) && !defined(unix)
			bKeyFound = NO,
			iFile_cnt = -1;
#else
			bKeyFound = NO;
#endif
static	char		szUserFile   [NAME_SIZE],
			szUserSection[NAME_SIZE];

register char		*pC;

switch (iCase) {

    case ABOUT_TO_BEGIN:
	bKeyFound = NO;
	*mpszUserReturned = Null;
	if (muiUserSize < 2)	/* buffer size must be at least 2 bytes */
	    return (STOP);
	memset(mpszUserReturned, Null, muiUserSize--); /* -- insures a string */
	if (bInSection and
		pfileConfig and
		pfileConfig == mpfileConfig and
#if	!defined(__TURBOC__) && !defined(unix)
		iFile_cnt == mpfileConfig->_cnt and
#endif
		no ferror(mpfileConfig) and
		IsStrEquiv(szUserFile	, mszUserFile	) and
		IsStrEquiv(szUserSection, mszUserSection) and
		fgets(mszFilePre, LINE_SIZE, mpfileConfig)) {	/* has '\n' */
	    StrTrunc(mszFilePre, " \t\r\n");
    /*   StrDetab(mszFilePre);  */
	    if (!IsStrNull(mszFilePre) and
		    *mszFilePre != ';') {
		if (pC = strstr(mszFilePre, " ;"))
		    *pC = Null; 			/* remove comments */
		StrCrop(mszFilePre, " ");               /* now "Key = String" */
		if (!IsStrNull(mszFilePre)) {	      /* or "Key=" or "Key" */
		    if (pC = strchr(mszFilePre, '=')) { /* if an equal sign */
			*pC++ = Null;			/* end Key */
			pC = StrSkip(pC, " ");          /* skip to String */
			StrTrunc(mszFilePre, " ");      /* truncate Key */
			}
		    if (IsStrEquiv(mszFilePre, mszUserKey)) {
			bKeyFound = YES;
			if (pC)
			    strncpy(mpszUserReturned, pC, muiUserSize);
			mfnGetConfigString(END_OF_PROCESS);	/* recursion! */
			if (gbConfigCloseFileOnExit)
			    CloseFile();
			return (STOP);
			}
		    }
		}
	    }
	strcpy(szUserFile   , mszUserFile   );
	strcpy(szUserSection, mszUserSection);
	bInSection = NO;
	break;

    case ARE_IN_SECTION:
	bInSection = YES;
	if (mpszFileKey and IsStrEquiv(mpszFileKey, mszUserKey)) {
	    bKeyFound = YES;
	    strncpy(mpszUserReturned, mpszFileString, muiUserSize);
	    if (IsStrNULL(mpszUserDefault))
		mpszUserDefault = mpszFileKey;
	    return (STOP);	/* Key was found */
	    }
	break;

    case NOT_IN_SECTION:
	if (bInSection) {
	    bInSection = NO;
	    return (STOP);	/* was in, but am now not */
	    }
	break;

    case END_OF_PROCESS:
	if (bKeyFound)
	    while (!IsStrNull(mpszUserReturned)) {
		pC = StrNull(mpszUserReturned) - 1;
		if (*pC != '\\')
		    break;
		*pC = '\0';
		if (no fgets(mszFilePre, LINE_SIZE, mpfileConfig))
		    break;					/* has '\n' */
		StrTrunc(mszFilePre, " \t\r\n");
        /*   StrDetab(mszFilePre);  */
		StrLeft(mszFilePre, " ");
		strncpy(pC, mszFilePre, muiUserSize - strlen(mpszUserReturned));
		}
	if (IsStrNull(mpszUserReturned))
	    strncpy(mpszUserReturned, (mpszUserDefault?  mpszUserDefault:
		    mszUserKey), muiUserSize);
	miReturn = strlen(mpszUserReturned);
#if	!defined(__TURBOC__) && !defined(unix)
	iFile_cnt = (pfileConfig = mpfileConfig)?  mpfileConfig->_cnt:	-1;
#endif
	break;		 /* the '=' in the above line is OK */
    }
return (CONTINUE);
}

/*p*/

/**
* function	WriteConfigString
*
* purpose	Similar to Windows "WriteProfileString".
*		Writes "KeyName=string" in the requested "SectionName"
*		(replacing an existing KeyName if it already exists).
*
* logic 	See "arguments" below.  The original file (if it exists) is
*		renamed to "*.BAK" after the new file is written.
*
* cautions	none.
*
* usage 	iSuccess = WriteConfigString(pszFilespec, pszSection, pszKey,
*			pszString);
*
* arguments	pszFilespec	"d:\\path\\filename.ext" format.
*				pszFilespec must not be NULL.
*		pszSection	"SectionName" of "[SectionName]".
*				pszSection may be NULL, implying "".
*				Blanks around pszSection are ignored.
*				Brackets are not included, but added herein.
*				If pszSection == "":
*					SectionNames in the file are ignored.
*		pszKey		"KeyName" of "KeyName=string".
*				pszKey may be NULL, implying " ".
*				Blanks around pszKey are ignored.
*				If pszKey == "":
*					A blank KeyName is written.
*		pszString	"string" of "KeyName=string".
*				If pszString is NULL, the old "KeyName=string"
*				is removed.  A Null ("") pszString is permitted.
*
* returns	iSuccess = TRUE if successful, FALSE if not.
*
* modifies	no externals.
*
* examples	iSuccess = WriteConfigString("C:\\CONFIG.SYS", NULL, "files",
*			"32");
*
* copyright	(c) Fran Finnegan Associates 1986, 1987
**/

extern	int		WriteConfigString(pszFilespec, pszSection, pszKey,
				pszString)

	char		*pszFilespec,
			*pszSection,
			*pszKey,
			*pszString;
{
mbWriteRequest = YES;
ParseKey(mszUserKey, pszKey);					/* Key	    */
mpszUserString = pszString;					/* String   */
ParseFile(pszFilespec, pszSection, mfnWriteConfigString);
mbWriteRequest = NO;
return (miReturn);
}

/*p*/

static	int		mfnWriteConfigString(iCase)

	int		iCase;
{
static	FILE		*pfileTMP;
static	int		bWritten;
static	char		*pszCrLf,
			szCrLf[] = "\n",
			szBAK[] = "BAK",
			szTMP[] = "TMP",
			szFILEformat[] = "%s\n";

auto	char		szFilespec[FILESPEC	 ],
			szDrive   [FILESPEC_DRIVE],
			szPath	  [FILESPEC_PATH ],
			szName	  [FILESPEC_NAME ],
			szExt	  [FILESPEC_EXT  ];

switch (iCase) {

    case ABOUT_TO_BEGIN:
	miReturn = NO;		/* used as a Section indicator */
	if (*mszUserFile == ' ')
	    return (STOP);	/* old Filespec can not be a blank (NULL) */
	ParseFilespec(mszUserFile, szDrive, szPath, szName, szExt);
	if (IsStrEquiv(szExt, szBAK) or IsStrEquiv(szExt, szTMP))
	    return (STOP);	/* old Filespec can not be *.BAK or *.TMP */
	if (!access(mszUserFile, ACCESS_EXISTS)) {	/* if exist *.old */
	    MakeFilespec(szFilespec, szDrive, szPath, szName, szBAK);
	    if (!access(szFilespec, ACCESS_EXISTS))	/* if exist *.BAK */
		if (access(szFilespec, ACCESS_UNLINK) or unlink(szFilespec))
		    return (STOP);		/* could not delete *.BAK */
	    }
	MakeFilespec(szFilespec, szDrive, szPath, szName, szTMP);
	if (!access(szFilespec, ACCESS_EXISTS) or	/* if exist *.TMP */
		no (pfileTMP = fopen(szFilespec, "wt")))  /* no new *.TMP */
	    return (STOP);			  /* could not open *.TMP */
	pszCrLf = "";
	bWritten = mpszUserString == NULL;  /* assume written for removal */
	break;

    case ARE_IN_SECTION:
	if (IsStrNull(mszFilePre))
	    break;
	if (*mszFilePost == '[') {
	    fprintf(pfileTMP, pszCrLf);
	    pszCrLf = szCrLf;
	    }
	miReturn = YES; 	/* used as a Section indicator */
	if (mpszFileKey and IsStrEquiv(mpszFileKey, mszUserKey)) { /* a match */
	    if (!IsStrNULL(mpszUserString)) {			 /* replace */
		strcpy(StrSkip(mszFilePre, " "),
			&mszUserKey[*mszUserKey == ' ']);
		strcat(mszFilePre, (*mszUserKey != ' ' and *mszFilePre == ' ')?
			" = ":  "=");
		strcat(mszFilePre, mpszUserString);
		}
	    bWritten++;  /* may be written here (in Section) multiple times */
	    }
	if (!(IsStrNULL(mpszUserString) and /* !(string NULL & it is a match) */
		mpszFileKey and IsStrEquiv(mpszFileKey, mszUserKey)))
	    fprintf(pfileTMP, szFILEformat, /*StrEntab()*/ mszFilePre); /* old/new */
	break;

    case NOT_IN_SECTION:
	if (IsStrNull(mszFilePre))
	    break;
	if (miReturn and	/* used as a Section indicator */
		not bWritten) { 				    /* append */
	    fprintf(pfileTMP, "%s=%s\n", &mszUserKey[*mszUserKey == ' '],
		    mpszUserString);				    /* add */
	    bWritten = YES;
	    }
	if (*mszFilePost == '[') {
	    fprintf(pfileTMP, pszCrLf);
	    pszCrLf = szCrLf;
	    }
	fprintf(pfileTMP, szFILEformat, /*StrEntab()*/ mszFilePre); /* old */
	break;

    case END_OF_PROCESS:
	if (no miReturn and	/* used as a Section indicator */
		!IsStrNull(mszUserSection)) {  /* never NULL */
	    fprintf(pfileTMP, pszCrLf);
	    fprintf(pfileTMP, szFILEformat, mszUserSection);	    /* add */
	    }
	if (not bWritten) {
	    strcpy(mszFilePre, &mszUserKey[*mszUserKey == ' ']);
	    strcat(mszFilePre, "=");
	    strcat(mszFilePre, mpszUserString);
	    fprintf(pfileTMP, szFILEformat,
		    /*StrEntab()*/ mszFilePre); 		    /* add */
	    }
#ifndef unix
	fprintf(pfileTMP, "%c", A_EOF);                             /* EOF */
#endif
	fclose(pfileTMP);
	ParseFilespec(mszUserFile, szDrive, szPath, szName, szExt);
	MakeFilespec(szFilespec, szDrive, szPath, szName, szBAK);
	rename(mszUserFile, szFilespec);	/* rename *.old *.BAK */
	if (gbConfigDeleteBakOnExit)
	    unlink(szFilespec);
	MakeFilespec(szFilespec, szDrive, szPath, szName, szTMP);
	rename(szFilespec, mszUserFile);	/* rename *.TMP *.old */
	miReturn = YES; 			/* OK */
	break;
    }
return (CONTINUE);
}

/*p*/

/**
* function	GetConfigSection
*
* purpose	Gets an entire Section.  Each line is placed in the "Returned"
*		buffer and terminated with a CR/LF (to be compatible with the
*		Windows "DrawText" function).  The buffer is Null-terminated.
*
* logic 	See "arguments" below.
*
* cautions	The "Returned" buffer must be at least 2 bytes long.  It should
*		be long enough to accomodate the longest expected Section.
*
* usage 	iLines = GetConfigSection(pszFilespec, pszSection, pszReturned,
*				  uiSize);
*
* arguments	pszFilespec	"d:\\path\\filename.ext" format.
*				pszFilespec must not be NULL.
*		pszSection	"SectionName" of "[SectionName]".
*				pszSection must be supplied and not be NULL.
*				Blanks around pszSection are ignored.
*				Brackets are not included, but added herein.
*		pszReturned	Returned-string buffer (at least 2 bytes).
*		uiSize		Returned-string buffer size.
*
* returns	iLines = number of lines found.  0 indicates that the Section
*				was not found or had no lines.
*
* modifies	no externals.
*
* examples	auto	char	szWinIniColors[512];
*
*		iWinIniColors = GetConfigSection("C:\\WIN\\SYS\\WIN.INI",
*			"Colors", szWinIniColors, sizeof(szWinIniColors));
*
* copyright	(c) Fran Finnegan Associates 1986, 1987
**/

extern	int		GetMemorySection(lpmemConfig, pszSection, pszReturned,
				uiSize)

	char		*lpmemConfig,
			*pszSection,
			*pszReturned;
	unsigned int	uiSize;
{
auto	int		iReturn;

CloseFile();
mlpmemConfig = lpmemConfig;
iReturn = GetConfigSection(NULL, pszSection, pszReturned, uiSize);
mlpmemConfig = (char *)NULL;
return (iReturn);
}

extern	int		GetConfigSection(pszFilespec, pszSection, pszReturned,
				uiSize)

	char		*pszFilespec,
			*pszSection,
			*pszReturned;
	unsigned int	uiSize;
{
mpszUserReturned = pszReturned; 				/* Returned */
muiUserSize = uiSize;						/* Size     */
ParseFile(pszFilespec, pszSection, mfnGetConfigSection);
return (miReturn);
}

/*p*/

static	int		mfnGetConfigSection(iCase)

	int		iCase;
{
register int		iLength;

auto	char		*pC;

switch (iCase) {

    case ABOUT_TO_BEGIN:
	miReturn = 0;		/* used as a line counter */
	*mpszUserReturned = Null;
	if (muiUserSize < 2 or IsStrNull(mszUserSection))
	    return (STOP);
	memset(mpszUserReturned, Null, muiUserSize--);
	break;

    case ARE_IN_SECTION:
	if (*mszFilePre != ';' and      /* ignore comment */
		miReturn++) {	/* used as a line counter */
	    if (pC = strstr(mszFilePre, " ;")) {
		*pC = Null;		/* remove comment */
		StrTrunc(mszFilePre, " ");
		}
	    iLength = strlen(strcat(mszFilePre, "\r\n"));
	    if (iLength > muiUserSize)
		mszFilePre[iLength = muiUserSize] = Null;
	    strcpy(mpszUserReturned, mszFilePre);
	    mpszUserReturned += iLength;  /* effectively concats */
	    if ((muiUserSize -= iLength) == 0)
		return (STOP);	/* no more room */
	    }
	break;

    case NOT_IN_SECTION:
	if (miReturn)		/* used as a line counter */
	    return (STOP);	/* was in, but am now not */
	break;

    case END_OF_PROCESS:
	if (miReturn)		/* was in */
	    miReturn--; 	/* ignore "[Section]" line */
	break;
    }
return (CONTINUE);
}

/*p*/

/**
* function	GetConfigKeys
*
* purpose	Gets a set of Keys.  Each key is placed in the "Returned"
*		buffer and terminated with a CR/LF (to be compatible with the
*		Windows "DrawText" function).  The buffer is Null-terminated.
*
* logic 	See "arguments" below.
*
* cautions	The "Returned" buffer must be at least 2 bytes long.  It should
*		be long enough to accomodate the longest expected set of Keys.
*
* usage 	iKeys = GetConfigKeys(pszFilespec, pszSection, pszReturned,
*			uiSize);
*
* arguments	pszFilespec	"d:\\path\\filename.ext" format.
*				pszFilespec must not be NULL.
*		pszSection	"SectionName" of "[SectionName]".
*				pszSection may be NULL, implying "".
*				Blanks around pszSection are ignored.
*				Brackets are not included, but added herein.
*		pszReturned	Returned-string buffer (at least 2 bytes).
*		uiSize		Returned-string buffer size.
*
* returns	iKeys = number of keys found.  0 indicates that the Section
*				was not found or no keys were found.
*
* modifies	no externals.
*
* examples	auto	char	szWinIniFonts[1 K];
*
*		iWinIniFonts = GetConfigKeys("C:\\WIN\\SYS\\WIN.INI",
*			"Fonts", szWinIniFonts, sizeof(szWinIniFonts));
*
* copyright	(c) Fran Finnegan Associates 1986, 1987
**/

extern	int		GetMemoryKeys(lpmemConfig, pszSection, pszReturned,
				uiSize)

	char		*lpmemConfig,
			*pszSection,
			*pszReturned;
	unsigned int	uiSize;
{
auto	int		iReturn;

CloseFile();
mlpmemConfig = lpmemConfig;
iReturn = GetConfigKeys(NULL, pszSection, pszReturned, uiSize);
mlpmemConfig = (char *)NULL;
return (iReturn);
}

extern	int		GetConfigKeys(pszFilespec, pszSection, pszReturned,
				uiSize)

	char		*pszFilespec,
			*pszSection,
			*pszReturned;
	unsigned int	uiSize;
{
mpszUserReturned = pszReturned; 				/* Returned */
muiUserSize = uiSize;						/* Size     */
ParseFile(pszFilespec, pszSection, mfnGetConfigKeys);
return (miReturn);
}

/*p*/

static	int		mfnGetConfigKeys(iCase)

	int		iCase;
{
register int		iLength;

switch (iCase) {

    case ABOUT_TO_BEGIN:
	miReturn = 0;		/* used as a key counter */
	*mpszUserReturned = Null;
	if (muiUserSize < 2)
	    return (STOP);
	memset(mpszUserReturned, Null, muiUserSize--);
	break;

    case ARE_IN_SECTION:
	if (mpszFileKey) {
	    miReturn++; 	/* used as a key counter */
#ifdef	oold
	    iLength = strlen(strcat(mpszFileKey, "\r\n"));      /* BEWARE! */
#else
	    iLength = strlen(mpszFileKey);
#endif
	    if (iLength > muiUserSize)
		mpszFileKey[iLength = muiUserSize] = Null;
	    strcpy(mpszUserReturned, mpszFileKey);
#ifndef oold
	    iLength++;	/* Append a '\000'      */
#endif
	    mpszUserReturned += iLength;  /* effectively concats */
	    if ((muiUserSize -= iLength) == 0)
		return (STOP);	/* no more room */
	    }
	break;

    case NOT_IN_SECTION:
	if (miReturn)		/* used as a key counter */
	    return (STOP);	/* was in, but am now not */
	break;
    }
return (CONTINUE);
}

/*p*/

static	void		CloseFile()
{
if (mpfileConfig) {
    fclose(mpfileConfig);
    mpfileConfig = NULL;
    }
}

/***/

static	void		ParseFile(pszFilespec, pszSection, pfnCallBack)

	char		*pszFilespec,
			*pszSection;
	int		(*pfnCallBack)(int);
{
static	char		szPreviousFile[FILESPEC];	/* previous request */
static	char		szBlankKey[] = " \r\n";         /* used for blank key */

register int		bFileUserSection;
register char		*pC;

ParseKey(mszUserFile, pszFilespec);				/* Filespec */
ParseSection(mszUserSection, pszSection);			/* Section  */
if ((*pfnCallBack)(ABOUT_TO_BEGIN) == CONTINUE) {
    if (mpfileConfig
    and IsStrEquiv(szPreviousFile, mszUserFile))
	rewind(mpfileConfig);
    else {
	CloseFile();
	if (no mlpmemConfig)
	    mpfileConfig = fopen(strcpy(szPreviousFile, mszUserFile), "rt");
	}
    if (mpfileConfig
    or	mlpmemConfig) {
	bFileUserSection = IsStrNull(mszUserSection);	/* in Section? */
	while (mlpmemConfig?
		MemoryGetString(mszFilePre, LINE_SIZE, &mlpmemConfig):
		fgets(mszFilePre, LINE_SIZE, mpfileConfig)) {	/* has '\n' */
	    StrTrunc(mszFilePre, " \t\r\n");
    /*   StrDetab(mszFilePre);  */
	    mpszFileKey = mpszFileString = NULL;  /* assume no "Key = String" */
	    *mszFilePost = Null;
	    if (!IsStrNull(mszFilePre)
	    and *mszFilePre != ';') {
		strcpy(mszFilePost, mszFilePre);
		if (pC = strstr(mszFilePost, " ;"))
		    *pC = Null; 	/* remove comments */
		StrCrop(mszFilePost, " ");      /* now "Key = String" */
		if (!IsStrNull(mszFilePost))
		    if (*mszFilePost == '[')    /* assume a "[SectionName]" */
			bFileUserSection = IsStrNull(mszUserSection)?  YES:
				IsStrEquiv(mszUserSection, mszFilePost);
		    else {			/* assume a "Key = string" */
			if (pC = strchr(mszFilePost, '=')) {
			    *pC++ = Null;
			    mpszFileString = StrSkip(pC, " ");
			    StrTrunc(mszFilePost, " ");
			    }
			else
			    mpszFileString = "";  /* BEWARE OF STRCATTING! */
			mpszFileKey = !IsStrNull(mszFilePost)?	mszFilePost:
				szBlankKey;	  /* BEWARE OF STRCATTING! */
			szBlankKey[1] = Null;	/* to allow for strcatting */
			}
		}
	    if ((*pfnCallBack)(bFileUserSection) == STOP)  /* do call-back */
		break;
	    }
	}
    (*pfnCallBack)(END_OF_PROCESS);	/* file may not have been read */
    if (mbWriteRequest or gbConfigCloseFileOnExit)
	CloseFile();
    }
}

/*p*/

static	void		ParseSection(pszTo, pszFrom)

	char		*pszTo,
			*pszFrom;
{
*pszTo++ = '[';                 /* add '[' */
if (pszFrom) {
    strncpy(pszTo, StrSkip(pszFrom, " "), NAME_SIZE - 3);
    StrTrunc(pszTo, " ");
    }
else
    *pszTo = Null;
if (IsStrNull(pszTo))		/* if pszTo == "[]", */
    *--pszTo = Null;		/*	SectionNames are to be ignored */
else
    strcat(pszTo, "]");         /* add ']' */
}

/***/

static	void		ParseKey(pszTo, pszFrom)

	char		*pszTo,
			*pszFrom;
{
if (pszFrom) {
    strncpy(pszTo, StrSkip(pszFrom, " "), NAME_SIZE - 1);
    StrTrunc(pszTo, " ");
    }
else
    *pszTo = Null;
if (IsStrNull(pszTo)) { 	/* if pszTo == "", */
    *pszTo++ = ' ';             /*      KeyNames must be at least " " */
    *pszTo = Null;
    }
}

/***/

static	char		*MemoryGetString(pszBuffer, iN, plpmemMemory)

	char		*pszBuffer;
register int		iN;
	char		**plpmemMemory;
{
register char		cChar;

auto	char		*pszString;

if (	**plpmemMemory == A_EOF or
	**plpmemMemory == Null	/* or
	no iN */  )
    return (NULL);
pszString = pszBuffer;
pszString[--iN] = Null;
while (iN--) {
    while ((cChar = **plpmemMemory) == '\r')
	++*plpmemMemory;
    if ((*pszString = cChar) == '\n')
	*++pszString = Null;
    if (cChar)
	++*plpmemMemory;
    if (no *pszString++)
	break;
    }
return (pszBuffer);
}

/**
* module	FFC\Filespec
*
* purpose	Parse and Make Filespecs.
*
* logic 	Simple string manipulation.
*
* cautions	All output-pointers to char arrays that are to be "filled in"
*		must have their memory allocated per #defines in "filespec.h".
*
* includes	<ffc/filespec.h>
*
* copyright	(c) Fran Finnegan Associates 1986
**/

#ifndef LINT_ARGS
#define LINT_ARGS
#endif

#define PATH		'\\'    /* PATH delimiter */
#define MAX_SPEC_CHARS	(FILESPEC - 1)		/* should be 63 */
#define MAX_PATH_CHARS	(FILESPEC_PATH - 1)	/* should be 62 */
#define MAX_NAME_CHARS	(FILESPEC_NAME - 1)	/* should be 8 */
#define MAX_EXT_CHARS	(FILESPEC_EXT - 2)	/* should be 3 */

static	char		mszFilespec[FILESPEC];
static	char		mszNull[] = "";
static	char		mszNUL[] = "NUL";

/*p*/

/**
* function	MakeFilespec
*
* purpose	Builds a filespec from supplied components.
*
* logic 	See commented code.
*
* cautions	A PATH gets suffixed to pszPath;  a '.' gets prefixed to pszExt.
*		All output-pointers to char arrays that are to be "filled in"
*		must have their memory allocated per #defines in "filespec.h".
*
* usage 	pszFilespec = MakeFilespec(pszFilespec, pszDrive, pszPath,
*			pszName, pszExt);
*
* arguments			(any of these may be NULL)
*		Output:
*			char	*pszFilespec;	 "d:\\path\\filename.ext"
*		Input:
*			char	*pszDrive;	 "d:", "d" or ""
*			char	*pszPath;	 "\\path\\", "\\", "",
*						 "\\path" or "path\\"
*			char	*pszName;	 "filename"
*			char	*pszExt;	 ".ext", "ext" or ""
*
* returns	pszFilespec = Filespec.
*
* modifies	no externals.
*
* examples	Obvious.
*
* copyright	(c) Fran Finnegan Associates 1986
**/

extern	char		*MakeFilespec(pszFilespec, pszDrive, pszPath, pszName,
				pszExt)

	char		*pszFilespec,
			*pszDrive,
			*pszPath,
			*pszName,
			*pszExt;
{
register char		*pc;

if (no pszFilespec)			/* allow Filespec to be NULL	*/
    pszFilespec = mszFilespec;
memset(pszFilespec, Null, FILESPEC);	/* zero out Filespec		*/
if (no pszDrive)			/* allow Drive to be NULL	*/
    pszDrive = mszNull;
if (no pszPath) 			/* allow Path to be NULL	*/
    pszPath = mszNull;
if (no pszName) 			/* allow Name to be NULL	*/
    pszName = mszNull;
if (no pszExt)				/* allow Ext to be NULL 	*/
    pszExt = mszNull;
					/********** Drive ***************/
pszFilespec[0] = *pszDrive;		/* begin with possible Drive	*/
pszFilespec[1] = ':';                   /* assume it was supplied       */
					/********** Path ****************/
if (!IsStrNull(pszPath)) {		/* if a Path was supplied:	*/
    strncat(pszFilespec, pszPath,	/*	add Path		*/
	    MAX_PATH_CHARS);		/*	  only the valid bytes	*/
    pc = StrNull(pszFilespec);		/*	find end-of-string	*/
    if (*(pc - 1) != PATH)		/*	see if it's not PATH:   */
	*pc = PATH;			/*		add PATH	*/
    }
					/********** Name ****************/
strncat(pszFilespec, pszName,		/* add Name			*/
	MAX_NAME_CHARS);		/*   only the valid bytes	*/
					/********** Ext *****************/
if (!IsStrNull(pszExt)) {		/* if an Ext was supplied:	*/
    strcat(pszFilespec, ".");           /*      add '.'                 */
    strncat(pszFilespec,		/*	add Ext 		*/
	    StrSkip(pszExt, "."),       /*        check for a '.':      */
	    MAX_EXT_CHARS);		/*	  only the valid bytes	*/
    }
					/********** Filespec ************/
if (IsStrNull(pszFilespec))		/* allow Filespec to be Null	*/
    strcat(pszFilespec, mszNUL);	/*	convert it to NUL:	*/
return (strupr(pszFilespec));		/* convert it to upper case	*/
}

/*p*/

/**
* function	ParseFilespec
*
* purpose	Breaks up a filespec into its components.
*
* logic 	See commented code.
*
* cautions	All output-pointers to char arrays that are to be "filled in"
*		must have their memory allocated per #defines in "filespec.h".
*
* usage 	ParseFilespec(pszFilespec, pszDrive, pszPath, pszName, pszExt);
*
* arguments			(any of these may be NULL)
*		Input:
*			char	*pszFilespec;	 "d:\\path\\filename.ext"
*		Output:
*			char	*pszDrive;	 "d:" or ""
*			char	*pszPath;	 "\\path", "\\" or ""
*			char	*pszName;	 "filename"
*			char	*pszExt;	 "ext" or ""
*
* returns	nothing.  Filespec components are passed via the arguments.
*
* modifies	no externals.
*
* examples	Obvious.
*
* copyright	(c) Fran Finnegan Associates 1986
**/

extern	void		ParseFilespec(pszFilespec, pszDrive, pszPath, pszName,
				pszExt)

	char		*pszFilespec,
			*pszDrive,
			*pszPath,
			*pszName,
			*pszExt;
{
register char		*pc;

auto	char		szSpec[FILESPEC];

if (no pszFilespec)			/* allow Filespec to be NULL	*/
    pszFilespec = mszNUL;
if (no pszDrive)			/* allow Drive to be NULL	*/
    pszDrive = mszFilespec;
if (no pszPath) 			/* allow Path to be NULL	*/
    pszPath = mszFilespec;
if (no pszName) 			/* allow Name to be NULL	*/
    pszName = mszFilespec;
if (no pszExt)				/* allow Ext to be NULL 	*/
    pszExt = mszFilespec;
memset(pszDrive, Null, FILESPEC_DRIVE); /* zero out Drive		*/
memset(pszPath , Null, FILESPEC_PATH ); /* zero out Path		*/
memset(pszExt  , Null, FILESPEC_EXT  ); /* zero out Ext 		*/
pszName[MAX_NAME_CHARS] =		/* end Name buffer		*/
 szSpec[MAX_SPEC_CHARS] = Null; 	/* end Spec buffer		*/
					/********** Filespec ************/
pszFilespec =				/* use the upper case version	*/
	strupr(strncpy(szSpec,		/*   convert it to upper case	*/
	pszFilespec, MAX_SPEC_CHARS));	/*   only the valid bytes	*/
					/********** Drive ***************/
if (pszFilespec[1] == ':') {            /* if it looks like a Drive:    */
    *pszDrive++ = *pszFilespec; 	/*	copy Drive		*/
    *pszDrive = ':';                    /*      add ':'                 */
    pszFilespec += 2;			/*	move past Drive 	*/
    }
					/********** Path ****************/
if (pc = strrchr(pszFilespec, PATH)) {	/* find last PATH in Filespec:	*/
    *pc++ = Null;			/*	end Path		*/
    strncpy(pszPath, pszFilespec,	/*	copy Path		*/
	    MAX_PATH_CHARS - 1);	/*	  only the valid bytes	*/
    if (IsStrNull(pszPath))		/*	take root into account: */
	*pszPath = PATH;		/*		set root	*/
    pszFilespec = pc;			/*	move past Path		*/
    }
					/********** Name ****************/
if (pc = strchr(pszFilespec, '.'))      /* find first '.' in Filespec:  */
    *pc++ = Null;			/*	end Name		*/
strncpy(pszName, pszFilespec,		/* copy Name			*/
	MAX_NAME_CHARS);		/*   only the valid bytes	*/
					/********** Ext *****************/
if (pc) 				/* if a '.' exists:             */
    strncpy(pszExt, pc, MAX_EXT_CHARS); /*	copy Ext		*/
}

/**
* name		StrTrunc -- truncate a string based on a set of trailing chars
*
* synopsis	str = StrTrunc(str, sub);
*
*		char	*str,		original string being truncated
*			*sub;		truncation-character string
*
* description	Removes each trailing character in "str" that is in "sub".
*		The string length is shortened by replacement with a null char.
*
* logic 	Obvious.
*
* returns	str = original string.
*
* modifies	External "CharASCII", temporarily.
*
* cautions	"str" and "sub" must be valid pointers.
*
* examples	Obvious.
**/

#ifndef LINT_ARGS
#define LINT_ARGS
#endif

/***/

extern	char		*StrTrunc(str, sub)

	char		*str,
			*sub;
{
register char  *s;

/*
*	set up CharASCII
*/

s = (unsigned char *)sub;
while (*s)
    CharASCII[*s++] = Null;

/*
*	truncate
*/

s = (unsigned char *)str;
while (*s)
    s++;
while (!(*s = CharASCII[*s]) && s-- > str)
    ;

/*
*	re-set CharASCII
*/

s = (unsigned char *)sub;
while (CharASCII[*s] = *s)
    s++;

return (str);
}

extern	int		main(int argc, char **argv);


#ifndef __MINGW32__
static char * strupr(char *str)
{
  int i;
  for(i = 0; i < strlen(str); i++)
    if(str[i] <= 'Z' && str[i]>='A') str[i] += 'a'-'A';
  return str;
}
#endif


#if 0
int stricmp(s1,s2)
char *s1, *s2;
{
    char ss1[100], ss2[100];
    strcpy(ss1, strupr(s1));
    strcpy(ss2, strupr(s2));
    return strcmp(ss1,ss2);
}
extern	int		main
(
	int		argc,
	char		**argv
)
{
static	char		szNUL[] = "NUL";

if (argc != 5 || *argv[1] == '-' || *argv[1] == '/' || *argv[1] == '?')
    {
    fprintf(stderr, Copyright, argv[0]);
    fprintf(stderr, "\nUsage:\t%s filespec \"section\" \"key\" \"string\"\n"
	    "where:\t\"section\", \"key\" and/or \"string\" may be \"nul\"\n"
	    "\tand must be enclosed in double-quotes if they contain blanks\a\n",
	    argv[0]);
    return 1;
    }

gbConfigCloseFileOnExit = YES;
gbConfigDeleteBakOnExit = NO;
/*
if (!WriteConfigString(argv[1],                 /*  File  */
      (!IsStrEquiv(argv[2], szNUL)?  argv[2]:  NULL),   /*  Section  */
      (!IsStrEquiv(argv[3], szNUL)?  argv[3]:  NULL),   /*  Key  */
     (!IsStrEquiv(argv[4], szNUL)?  argv[4]:  NULL)))  /*  String  */
*/
if (!WriteConfigString(argv[1],                 /*  File  */
      (!IsStrEquiv(argv[2], szNUL)?  argv[2]:  argv[2]),   /*  Section  */
      (!IsStrEquiv(argv[3], szNUL)?  argv[3]:  argv[3]),   /*  Key  */
     (!IsStrEquiv(argv[4], szNUL)?  argv[4]:  argv[4])))  /*  String  */
    {
    fprintf(stderr, Copyright, argv[0]);
    fprintf(stderr, "\nError updating %s\a\n", argv[1]);
    return 2;
    }
return 0;
}


main()
{
   char buffer[100];
   GetConfigString("c:\\windows\\fwt.ini", "Addr", "HostIp", "12345", buffer, 80);
   printf(buffer);
}
#endif

