/*	============
 *	test-time.cc
 *	============
 */

// Standard C
#include <time.h>

// Iota
#include "iota/strings.hh"

// poseven
#include "poseven/functions/write.hh"

// Orion
#include "Orion/Main.hh"


namespace tool
{
	
	namespace p7 = poseven;
	
	
	#define RUN_TEST( condition, i )  run_test( condition, STR_LEN( "ok " #i "\n" ) )
	
	static void run_test( bool condition, const char* ok, size_t ok_length )
	{
		if ( !condition )
		{
			p7::write( p7::stdout_fileno, STR_LEN( "not " ) );
		}
		
		p7::write( p7::stdout_fileno, ok, ok_length );
	}
	
	
	static bool time_matches( const struct tm&  modular,
	                          int               year,
	                          int               month,
	                          int               mday )
	{
		return modular.tm_year + 1900 == year
		    && modular.tm_mon  +    1 == month
		    && modular.tm_mday        == mday;
	}
	
	static bool time_matches( const time_t&  date,
	                          int            year,
	                          int            month,
	                          int            mday )
	{
		struct tm modular_time = { 0 };
		
		return gmtime_r( &date,
		                 &modular_time ) && time_matches( modular_time,
		                                                  year,
		                                                  month,
		                                                  mday );
	}
	
	
	int Main( int argc, char** argv )
	{
		p7::write( p7::stdout_fileno, STR_LEN( "1..50\n" ) );
		
		//time_t now = time( NULL );
		
		const unsigned seconds_per_day = 60 * 60 * 24;
		
		time_t epoch = 0;
		
		RUN_TEST( time_matches( epoch,                         1970,  1,  1 ),  1 );
		RUN_TEST( time_matches( epoch + seconds_per_day *  30, 1970,  1, 31 ),  2 );
		RUN_TEST( time_matches( epoch + seconds_per_day *  31, 1970,  2,  1 ),  3 );
		RUN_TEST( time_matches( epoch + seconds_per_day *  58, 1970,  2, 28 ),  4 );
		RUN_TEST( time_matches( epoch + seconds_per_day *  59, 1970,  3,  1 ),  5 );
		RUN_TEST( time_matches( epoch + seconds_per_day *  89, 1970,  3, 31 ),  6 );
		RUN_TEST( time_matches( epoch + seconds_per_day *  90, 1970,  4,  1 ),  7 );
		RUN_TEST( time_matches( epoch + seconds_per_day * 119, 1970,  4, 30 ),  8 );
		RUN_TEST( time_matches( epoch + seconds_per_day * 120, 1970,  5,  1 ),  9 );
		RUN_TEST( time_matches( epoch + seconds_per_day * 150, 1970,  5, 31 ), 10 );
		RUN_TEST( time_matches( epoch + seconds_per_day * 151, 1970,  6,  1 ), 11 );
		RUN_TEST( time_matches( epoch + seconds_per_day * 180, 1970,  6, 30 ), 12 );
		RUN_TEST( time_matches( epoch + seconds_per_day * 181, 1970,  7,  1 ), 13 );
		RUN_TEST( time_matches( epoch + seconds_per_day * 211, 1970,  7, 31 ), 14 );
		RUN_TEST( time_matches( epoch + seconds_per_day * 212, 1970,  8,  1 ), 15 );
		RUN_TEST( time_matches( epoch + seconds_per_day * 242, 1970,  8, 31 ), 16 );
		RUN_TEST( time_matches( epoch + seconds_per_day * 243, 1970,  9,  1 ), 17 );
		RUN_TEST( time_matches( epoch + seconds_per_day * 272, 1970,  9, 30 ), 18 );
		RUN_TEST( time_matches( epoch + seconds_per_day * 273, 1970, 10,  1 ), 19 );
		RUN_TEST( time_matches( epoch + seconds_per_day * 303, 1970, 10, 31 ), 20 );
		RUN_TEST( time_matches( epoch + seconds_per_day * 304, 1970, 11,  1 ), 21 );
		RUN_TEST( time_matches( epoch + seconds_per_day * 333, 1970, 11, 30 ), 22 );
		RUN_TEST( time_matches( epoch + seconds_per_day * 334, 1970, 12,  1 ), 23 );
		RUN_TEST( time_matches( epoch + seconds_per_day * 364, 1970, 12, 31 ), 24 );
		
		time_t year_1971 = epoch + seconds_per_day * 365;
		
		RUN_TEST( time_matches( year_1971,                        1971,  1,  1 ), 25 );
		RUN_TEST( time_matches( year_1971 + seconds_per_day * 59, 1971,  3,  1 ), 26 );
		
		time_t year_1972 = year_1971 + seconds_per_day * 365;
		
		RUN_TEST( time_matches( year_1972,                        1972,  1,  1 ), 27 );
		RUN_TEST( time_matches( year_1972 + seconds_per_day * 59, 1972,  2, 29 ), 28 );
		
		time_t year_1973 = year_1972 + seconds_per_day * 366;
		
		RUN_TEST( time_matches( year_1973,                        1973,  1,  1 ), 29 );
		RUN_TEST( time_matches( year_1973 + seconds_per_day * 58, 1973,  2, 28 ), 30 );
		RUN_TEST( time_matches( year_1973 + seconds_per_day * 59, 1973,  3,  1 ), 31 );
		
		time_t year_1974 = year_1973 + seconds_per_day * 365;
		
		RUN_TEST( time_matches( year_1974,                        1974,  1,  1 ), 32 );
		RUN_TEST( time_matches( year_1974 + seconds_per_day * 58, 1974,  2, 28 ), 33 );
		RUN_TEST( time_matches( year_1974 + seconds_per_day * 59, 1974,  3,  1 ), 34 );
		
		time_t year_1996 = year_1972 + seconds_per_day * (365 * (1996 - 1972) + (1996 - 1972) / 4);
		
		RUN_TEST( time_matches( year_1996,                        1996,  1,  1 ), 35 );
		RUN_TEST( time_matches( year_1996 + seconds_per_day * 59, 1996,  2, 29 ), 36 );
		RUN_TEST( time_matches( year_1996 + seconds_per_day * 60, 1996,  3,  1 ), 37 );
		
		time_t year_2000 = year_1996 + seconds_per_day * (365 * 4 + 1);
		
		RUN_TEST( time_matches( year_2000,                        2000,  1,  1 ), 38 );
		RUN_TEST( time_matches( year_2000 + seconds_per_day * 59, 2000,  2, 29 ), 39 );
		RUN_TEST( time_matches( year_2000 + seconds_per_day * 60, 2000,  3,  1 ), 40 );
		
		time_t year_2004 = year_2000 + seconds_per_day * (365 * 4 + 1);
		
		RUN_TEST( time_matches( year_2004,                        2004,  1,  1 ), 41 );
		RUN_TEST( time_matches( year_2004 + seconds_per_day * 59, 2004,  2, 29 ), 42 );
		RUN_TEST( time_matches( year_2004 + seconds_per_day * 60, 2004,  3,  1 ), 43 );
		
		time_t year_2008 = year_2004 + seconds_per_day * (365 * 4 + 1);
		
		RUN_TEST( time_matches( year_2008,                        2008,  1,  1 ), 44 );
		RUN_TEST( time_matches( year_2008 + seconds_per_day * 59, 2008,  2, 29 ), 45 );
		RUN_TEST( time_matches( year_2008 + seconds_per_day * 60, 2008,  3,  1 ), 46 );
		
		time_t year_2009 = year_2008 + seconds_per_day * (365 + 1);
		
		RUN_TEST( time_matches( year_2009 - seconds_per_day,      2008, 12, 31 ), 47 );
		RUN_TEST( time_matches( year_2009,                        2009,  1,  1 ), 48 );
		RUN_TEST( time_matches( year_2009 + seconds_per_day * 58, 2009,  2, 28 ), 49 );
		RUN_TEST( time_matches( year_2009 + seconds_per_day * 59, 2009,  3,  1 ), 50 );
		
		
		return 0;
	}
	
}

