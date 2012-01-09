/*	======
 *	time.h
 *	======
 */

#ifndef KEROSENE_TIME_H
#define KEROSENE_TIME_H

#include <sys/types.h>


#define CLOCKS_PER_SEC  1000000ul

#define NANOSLEEP_DOZE  (-1)


#ifdef __cplusplus
extern "C" {
#endif
	
	struct tm
	{
		int tm_sec;
		int tm_min;
		int tm_hour;
		int tm_mday;
		int tm_mon;
		int tm_year;
		int tm_wday;
		int tm_yday;
		int tm_isdst;
		long tm_gmtoff;
		char* tm_zone;
	};
	
#ifndef _TIMESPEC_DECLARED
#define _TIMESPEC_DECLARED
	
	struct timespec
	{
		time_t	tv_sec;		/* seconds */
		long	tv_nsec;	/* and nanoseconds */
	};
	
#endif
	
	char* ctime( const time_t* time );
	
	clock_t clock( void );
	struct tm* gmtime( const time_t* );
	struct tm* gmtime_r( const time_t*, struct tm *result );
	struct tm* localtime( const time_t* );
	struct tm* localtime_r( const time_t*, struct tm *result );
	
	time_t mktime( struct tm *tm );
	
	int gettimeofday( struct timeval *tv, struct timezone *tz );
	int settimeofday( const struct timeval *tv , const struct timezone *tz );
	
	time_t time( time_t* t );
	
	int nanosleep( const struct timespec* requested, struct timespec* remaining );
	
	int doze( struct timespec const  *minimum,
	          struct timespec const  *maximum,
	          struct timespec        *remaining );
	
	size_t strftime( char *s, size_t max, const char *format, const struct tm *tm );
	
	char *strptime( const char *s, const char *format, struct tm *tm );
	
#ifdef __cplusplus
}
#endif

#endif

