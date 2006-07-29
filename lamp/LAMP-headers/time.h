/*	======
 *	time.h
 *	======
 */

#ifndef KEROSENE_TIME_H
#define KEROSENE_TIME_H

#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif
	
	//#define CLOCKS_PER_SEC 1000000
	
	typedef unsigned long time_t;
	typedef unsigned long long clock_t;
	
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
	
	/*
	 * Structure returned by gettimeofday(2) system call,
	 * and used in other calls.
	 */
	struct timeval
	{
		time_t		tv_sec;		/* seconds */
		suseconds_t	tv_usec;	/* and microseconds */
	};
	
	char* ctime( const time_t* time );
	
	clock_t clock( void );
	struct tm* gmtime( const time_t* );
	struct tm* localtime( const time_t* );
	
	int gettimeofday( struct timeval *tv, struct timezone *tz );
	int settimeofday( const struct timeval *tv , const struct timezone *tz );
	
	time_t time( time_t* t );
	
#ifdef __cplusplus
}
#endif

#endif

