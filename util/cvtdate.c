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

#define  JULIAN_ADJUSTMENT    1721425L


static	char  month[][10]= {"         ",
			    "January  ",
			    "February ",
			    "March    ",
			    "April    ",
			    "May      ",
			    "June     ",
			    "July     ",
			    "August   ",
			    "September",
			    "October  ",
			    "November ",
			    "December "
			  } ;



/*  static c4julian

       Returns an (int) day of the year starting from 1.
       Ex.    Jan 1, returns  1

       Returns	-1  if it is an illegal date.

*/

static int  month_tot[]=
    { 0, 0,  31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365 } ;
	 /* Jan Feb Mar  Apr  May  Jun	Jul  Aug  Sep  Oct  Nov  Dec
	     31  28  31   30   31   30	 31   31   30	31   30   31
	 */

static	int c4julian( int year, int month, int day )
{
    int  is_leap, month_days ;

    is_leap =	( (year%4 == 0 && year%100 != 0) || year%400 == 0 ) ?  1 : 0 ;

    month_days =  month_tot[ month+1 ] -  month_tot[ month] ;
    if ( month == 2 )  month_days += is_leap ;

    if ( year  < 0  ||
	month < 1  ||  month > 12  ||
	day   < 1  ||  day   > month_days )
	return( -1 ) ;	/* Illegal Date */

    if ( month <= 2 )  is_leap = 0 ;

    return(  month_tot[month] + day + is_leap ) ;
}


/*   c4ytoj -  Calculates the number of days to the year */
static long  c4ytoj( int yr )
{
    /*
       This calculation takes into account the fact that
	  1)  Years divisible by 400 are always leap years.
	  2)  Years divisible by 100 but not 400 are not leap years.
	  3)  Otherwise, years divisible by four are leap years.

       Since we do not want to consider the current year, we will
       subtract the year by 1 before doing the calculation.
    */

    yr-- ;
    return( yr*365L +  yr/4L - yr/100L + yr/400L ) ;
}


dword	ymd2date(int years, int months, int day)
{
long	ret;
    ret = c4julian(years, months, day);
    if (ret < 0) return ret;
    ret += c4ytoj(years);
    ret += JULIAN_ADJUSTMENT;
    return ret;
}


/* Converts from formatted Date Data long julian Date format */
dword	str2date(LPSTR picture, LPSTR date_data)
{
    LPSTR	 month_start;
    char	 month_data[10] ;
    int   year_count, month_count, day_count, century_count, i, len ;
    int  months, day, years;
    long ret;
    char  buffer[30] ;
    char ybuff[20];

    if (date_data == 0 || date_data[0] == 0) return 0;
    day_count	 =  7 ;
    month_count  =  4 ;
    year_count	 =  1 ;
    century_count= -1 ;

    memset( buffer, (int) 0, (size_t) 12 ) ;
    memset( ybuff, 0, sizeof(ybuff) );

    for ( i=0; picture[i] != '\000'; i++ )
    {
      switch( picture[i] )
      {
	 case 'D':
	    if ( ++day_count >= 10) break ;
	    buffer[day_count] =  date_data[i] ;
	    break ;

	 case 'M':
	    if ( ++month_count >=7) break ;
	    buffer[month_count] =  date_data[i] ;
	    break ;

	 case 'Y':
	    if (year_count <= 6) ybuff[year_count] = date_data[i];
	    if ( ++year_count >= 4) break ;
	    buffer[year_count] =  date_data[i] ;
	    break ;

	 case 'C':
	    if ( ++century_count >= 2) break ;
	    buffer[century_count] =  date_data[i] ;
	    break ;
      }
    }

    // We assume always exist date chars
//    if ( strcmp( buffer, "        " ) == 0 )  return( buffer ) ;

    if ( year_count >= 4 ) memcpy( buffer, ybuff+1, 4); else {
	if ( century_count ==  -1 )  memcpy( buffer,   "19", (size_t) 2 ) ;
	if ( year_count    ==	1 )  memcpy( buffer+2, "01", (size_t) 2 ) ;
    }
    if ( month_count   ==   4 )  memcpy( buffer+5, "01", (size_t) 2 ) ;
    if ( day_count     ==   7 )  memcpy( buffer+8, "01", (size_t) 2 ) ;

    if ( month_count >= 7 )
    {
	/* Convert the Month from Character Form to Date Format */
	month_start =  strchr( picture, 'M' ) ;

	len =  month_count - 3 ;  /* Number of 'M' characters in picture */

	memcpy( buffer+4, "  ", (size_t) 2 ) ;

	if ( len > 3 ) len = 3 ;
	memcpy( month_data, (LPSTR)(date_data+ (int) (month_start-picture)), (size_t) len) ;
	while ( len > 0 )
	 if ( month_data[len-1] == ' ' )
	    len-- ;
	 else
	    break ;

	month_data[len] =  '\000' ;

	strlwr( month_data ) ;
	month_data[0] &= ~0x20;    // toupper

	months = 1;
	if ( len > 0 )
	 for( i=1; i<= 12; i++ )
	    if ( memcmp( month[i], month_data,	len) == 0)
	    {
	       months = i;
	       break ;
	    }
    } else months = atoi(buffer+5);
    years = atoi(buffer);
    day = atoi(buffer+8);

    ret = c4julian(years, months, day);
    if (ret < 0) return ret;
    ret += c4ytoj(years);
    ret += JULIAN_ADJUSTMENT;
    return ret;
}


/*  c4mon_dy

       Given the year and the day of the year, returns the
    month and day of month.
*/
static int  c4mon_dy( int year, int days,  unsigned *month_ptr,  unsigned *day_ptr )
{
    int is_leap, i ;

    is_leap =  ( (year%4 == 0 && year%100 != 0) || year%400 == 0 ) ?  1 : 0 ;
    if ( days <= 59 )  is_leap = 0 ;

    for( i = 2; i <= 13; i++)
    {
      if ( days <=  month_tot[i] + is_leap )
      {
	*month_ptr =  --i ;
	if ( i <= 2) is_leap = 0 ;

	*day_ptr   =  days - month_tot[ i] - is_leap ;
	return( 0) ;
      }
    }
    *day_ptr   =  0 ;
    *month_ptr =  0 ;

    return( -1 ) ;
}


/*   kmdate

     Converts from the julian date format to year/month/day format.
*/
static	struct	mytm	ktm;

struct	mytm	*kmdate_b(dword tot_days, struct mytm *tmp)
{
int  n_days, max_days ;

    /* A dBASE III index file date is stored as a julian day.  That is the
      number of days since the date  Jan 1, 4713 BC
      Ex.  Jan 1, 1981 is  2444606
    */

    if (tot_days <= 0) {
	tmp->year = 0;
	tmp->month = 1;
	tmp->day = 1;
	return tmp;
    }

    tot_days	-=  JULIAN_ADJUSTMENT ;

//    tmp->year        =  (int) ((double)tot_days/365.2425) + 1 ;
    tmp->year	 = tot_days * 400 / 146097l;
    n_days	=  (int) (tot_days -  c4ytoj(tmp->year)) ;
    if ( n_days <= 0 )
    {
	tmp->year-- ;
	n_days	 =  (int) (tot_days - c4ytoj(tmp->year)) ;
    }
    if ((tmp->year%4 == 0 && tmp->year%100 != 0) || tmp->year%400 == 0)
	max_days =  366 ;
    else
	max_days =  365 ;

    if ( n_days > max_days )
    {
	tmp->year++ ;
	n_days -= max_days ;
    }

    if ( c4mon_dy( tmp->year, n_days, &tmp->month, &tmp->day ) < 0 ) {
	tmp->year = 0;
	tmp->month = 1;
	tmp->day = 1;
	return tmp;
    }

    return tmp;
}


struct	mytm	*kmdate(dword tot_days)
{
    return kmdate_b(tot_days, &ktm);
}


static int  c4mon_end( int year, int days )
{
int is_leap, i ;

    is_leap =  ( (year%4 == 0 && year%100 != 0) || year%400 == 0 ) ?  1 : 0 ;
    if ( days <= 59 )  is_leap = 0 ;

    for( i = 2; i <= 13; i++)
    {
	if ( days <=  month_tot[i] + is_leap )
	{
		return( month_tot[i] + is_leap - days) ;
	}
    }
    return 0;
}


static	dword	monthend(dword tot_d)
{
int  n_days, max_days, year ;
long	     tot_days=tot_d;

    /* A dBASE III index file date is stored as a julian day.  That is the
      number of days since the date  Jan 1, 4713 BC
      Ex.  Jan 1, 1981 is  2444606
    */

    if (tot_days == 0) return 0; // invalid date

    tot_days	-=  JULIAN_ADJUSTMENT ;

    year    = tot_days * 400 / 146097l;
//    year	  =  (int) ((double)tot_days/365.2425) + 1 ;
    n_days	=  (int) (tot_days -  c4ytoj(year)) ;
    if ( n_days <= 0 )
    {
	year-- ;
	n_days	 =  (int) (tot_days - c4ytoj(year)) ;
    }
    if ((year%4 == 0 && year%100 != 0) || year%400 == 0)
	max_days =  366 ;
    else
	max_days =  365 ;

    if ( n_days > max_days )
    {
	year++ ;
	n_days -= max_days ;
    }

    return tot_d + c4mon_end( year, n_days );
}


dword	End_Month(dword days)
{
    return monthend(days);
}


LPSTR	Time2String_b(dword timev, LPSTR buff)
{
int	hour,min,sec;

    sec = timev % 60;
    timev /= 60;
    min = timev % 60;
    hour = timev / 60;
    wsprintf(buff, "%02d:%02d:%02d", hour, min, sec);
    return buff;
}

LPSTR	Time2String_d(dword timev)
{
static	char	buff[32];
int	hour,min,sec;
int	days;

    sec = timev % 60;
    timev /= 60;
    min = timev % 60;
    timev /= 60;
    hour = timev % 24;
    days = timev / 24;
    wsprintf(buff, "%d days %02d:%02d:%02d", days, hour, min, sec);
    return buff;
}


LPSTR	Time2String(dword timev)
{
static	char	buff[32];
    return Time2String_b(timev, buff);
}


int	days_of_year(dword tot_d)
{
int  n_days, max_days, year ;
long	     tot_days=tot_d;

    /* A dBASE III index file date is stored as a julian day.  That is the
      number of days since the date  Jan 1, 4713 BC
      Ex.  Jan 1, 1981 is  2444606
    */

    if (tot_days == 0) return 0; // invalid date

    tot_days	-=  JULIAN_ADJUSTMENT ;

    year    = tot_days * 400 / 146097l;
//    year	  =  (int) ((double)tot_days/365.2425) + 1 ;
    n_days	=  (int) (tot_days -  c4ytoj(year)) ;
    if ( n_days <= 0 )
    {
	year-- ;
	n_days	 =  (int) (tot_days - c4ytoj(year)) ;
    }
    if ((year%4 == 0 && year%100 != 0) || year%400 == 0)
	max_days =  366 ;
    else
	max_days =  365 ;

    if ( n_days > max_days )
    {
	year++ ;
	n_days -= max_days ;
    }
    return n_days;
}


void	date2str(dword _date, LPSTR bbuf)
{
struct	mytm	*tmp=kmdate(_date);

    if (_date == 0) {
	bbuf[0] = 0;
	return;
    }
    wsprintf(bbuf, "%04d%02d%02d", tmp->year, tmp->month, tmp->day);
}
