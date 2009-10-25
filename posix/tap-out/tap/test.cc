/*
	test.cc
	
	Copyright 2009, Joshua Juran
*/

#include "tap/test.hh"

// Standard C++
#include <algorithm>

// Standard C
#include <stdlib.h>
#include <stdio.h>

// POSIX
#include <unistd.h>

// Iota
#include "iota/decimal.hh"
#include "iota/strings.hh"

// more-posix
#include "more/perror.hh"

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
		
		fprintf( stderr, "There were %d %s tests\n",
									 std::abs( extra_tests ),
										extra_tests > 0 ? "extra"
														: "missing" );
		
		fflush( stderr );
		
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
		const char* separator = comment ? " - " : "";
		
		if ( comment == NULL )
		{
			comment = "";
		}
		
		printf( "%s" "ok %d" "%s" "%s\n",
		         ok ? ""
		            : "not ",
		                 ++global_tests_run,
		                      separator,
		                           comment );
	}
	
}

