/*
	test.cc
	
	Copyright 2009, Joshua Juran
*/

#include "tap/test.hh"

// Standard C++
#include <algorithm>

// Standard C
#include <stdlib.h>

// POSIX
#include <unistd.h>
#include <sys/uio.h>

// Iota
#include "iota/decimal.hh"
#include "iota/strings.hh"

// tap-out
#include "tap/check.hh"


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
		
		const unsigned odd_tests = std::abs( extra_tests );
		
		const unsigned magnitude = iota::decimal_magnitude( odd_tests );
		
		const char* tests_str = iota::inscribe_unsigned_decimal( odd_tests );
		
		const bool extra = extra_tests > 0;
		
		const bool missing = extra_tests < 0;
		
		struct iovec iov[] =
		{
			{ (void*) STR_LEN( "There were " )           },
			{ (void*) tests_str, magnitude               },
			{ (void*) STR_LEN( " "           )           },
			{ (void*) STR_LEN( "extra"       ) * extra   },
			{ (void*) STR_LEN( "missing"     ) * missing },
			{ (void*) STR_LEN( " tests\n"    )           },
		};
		
		CHECK( writev( STDERR_FILENO, iov, sizeof iov / sizeof iov[0] ) );
		
		_exit( 1 );
	}
	
	void start( const char* program_name, unsigned tests_planned )
	{
		set_program_name( program_name );
		
		char plan_string[ sizeof "1..1234567890\n" ] = "1..";
		
		char* p_last = plan_string + STRLEN( "1.." );
		
		char* p_lf = iota::inscribe_unsigned_decimal_r( tests_planned, p_last );
		
		*p_lf = '\n';
		
		const size_t length = p_lf + 1 - plan_string;
		
		CHECK( write( STDOUT_FILENO, plan_string, length ) );
		
		global_tests_planned = tests_planned;
		
		atexit( &atexit_count_tests );
	}
	
	void ok_if( bool ok, const char* comment )
	{
		if ( comment == NULL )
		{
			comment = "";
		}
		
		const bool not_ok = !ok;
		
		const unsigned magnitude = iota::decimal_magnitude( ++global_tests_run );
		
		const char* tests_str = iota::inscribe_unsigned_decimal( global_tests_run );
		
		const bool has_comment = comment[0] != '\0';
		
		struct iovec iov[] =
		{
			{ (void*) STR_LEN( "not " ) * not_ok      },
			{ (void*) STR_LEN( "ok "  )               },
			{ (void*) tests_str, magnitude            },
			{ (void*) STR_LEN( " - "  ) * has_comment },
			{ (void*) comment,   strlen( comment )    },
			{ (void*) STR_LEN( "\n"   )               },
		};
		
		CHECK( writev( STDOUT_FILENO, iov, sizeof iov / sizeof iov[0] ) );
		
	}
	
}

