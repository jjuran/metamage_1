/*
	test.cc
	
	Copyright 2009, Joshua Juran
*/

#include "tap/test.hh"

// Standard C
#include <stdlib.h>
#include <string.h>

// POSIX
#include <unistd.h>
#include <sys/uio.h>

// gear
#include "gear/inscribe_decimal.hh"

// tap-out
#include "tap/check.hh"


#pragma exceptions off


namespace tap
{
	
	static int global_tests_planned = 0;
	static int global_tests_run     = 0;
	
	
	static void atexit_count_tests()
	{
		const int extra_tests = global_tests_run - global_tests_planned;
		
		if ( extra_tests == 0 )
		{
			return;
		}
		
		const unsigned odd_tests = abs( extra_tests );
		
		const unsigned magnitude = gear::decimal_magnitude( odd_tests );
		
		const char* tests_str = gear::inscribe_unsigned_decimal( odd_tests );
		
		const bool extra = extra_tests > 0;
		
		const bool missing = extra_tests < 0;
		
		struct iovec iov[] =
		{
			{ (char*) STR_LEN( "There were " )           },
			{ (char*) tests_str, magnitude               },
			{ (char*) STR_LEN( " "           )           },
			{ (char*) STR_LEN( "extra"       ) * extra   },
			{ (char*) STR_LEN( "missing"     ) * missing },
			{ (char*) STR_LEN( " tests\n"    )           },
		};
		
		CHECK( writev( STDERR_FILENO, iov, sizeof iov / sizeof iov[0] ) );
		
		_exit( 1 );
	}
	
	void start( const char* program_name, unsigned tests_planned )
	{
		set_program_name( program_name );
		
		char plan_string[ sizeof "1..1234567890\n" ] = "1..";
		
		char* p_last = plan_string + STRLEN( "1.." );
		
		char* p_lf = gear::inscribe_unsigned_decimal_r( tests_planned, p_last );
		
		*p_lf = '\n';
		
		const size_t length = p_lf + 1 - plan_string;
		
		CHECK( write( STDOUT_FILENO, plan_string, length ) );
		
		global_tests_planned = tests_planned;
		
		atexit( &atexit_count_tests );
	}
	
	void print( bool ok )
	{
		const bool not_ok = !ok;
		
		const unsigned magnitude = gear::decimal_magnitude( ++global_tests_run );
		
		const char* tests_str = gear::inscribe_unsigned_decimal( global_tests_run );
		
		struct iovec iov[] =
		{
			{ (char*) STR_LEN( "not " ) * not_ok      },
			{ (char*) STR_LEN( "ok "  )               },
			{ (char*) tests_str, magnitude            },
			{ (char*) STR_LEN( "\n"   )               },
		};
		
		CHECK( writev( STDOUT_FILENO, iov, sizeof iov / sizeof iov[0] ) );
		
	}
	
	void expect( bool condition, const char* ref, unsigned len )
	{
		if ( !condition )
		{
			write( STDERR_FILENO, ref, len );
		}
	
		print( condition );
	}
	
	int cmp( const void* p, unsigned p_len, const void* q, unsigned q_len )
	{
		int result = q_len - p_len;
		
		if ( result == 0 )
		{
			result = memcmp( p, q, p_len );
		}
		
		if ( result != 0 )
		{
			log( p, p_len, q, q_len );
		}
		
		return result;
	}
	
}
