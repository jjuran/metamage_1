/*	======
 *	time.h
 *	======
 */

#ifndef _TIME_H_
#define _TIME_H_

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
	
	struct timespec
	{
		time_t	tv_sec;		/* seconds */
		long	tv_nsec;	/* and nanoseconds */
	};
	
	char* ctime( const time_t* time );
	
	clock_t clock( void );
	struct tm* gmtime( const time_t* );
	struct tm* gmtime_r( const time_t*, struct tm *result );
	struct tm* localtime( const time_t* );
	struct tm* localtime_r( const time_t*, struct tm *result );
	
	time_t mktime( struct tm *tm );
	
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
