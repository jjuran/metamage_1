/*
	test.cc
	
	Copyright 2009, Joshua Juran
*/

#include "tap/test.hh"

// Standard C
#include <stdlib.h>

// POSIX
#include <unistd.h>
#include <sys/uio.h>

// more-libc
#include "more/string.h"

// gear
#include "gear/hexidecimal.hh"
#include "gear/inscribe_decimal.hh"

// tap-out
#include "tap/check.hh"


#pragma exceptions off


namespace tap
{
	
	static int global_tests_planned = 0;
	static int global_tests_run     = 0;
	
	
	static inline size_t max( size_t a, size_t b )
	{
		return a > b ? a : b;
	}
	
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
		
		char* p_lf = gear::inscribe_unsigned_decimal_r( tests_planned, p_last );
		
		*p_lf = '\n';
		
		const size_t length = p_lf + 1 - plan_string;
		
		CHECK( write( STDOUT_FILENO, plan_string, length ) );
		
		global_tests_planned = tests_planned;
		
		atexit( &atexit_count_tests );
	}
	
	void print( bool ok, const char* comment )
	{
		if ( comment == NULL )
		{
			comment = "";
		}
		
		const bool not_ok = !ok;
		
		const unsigned magnitude = gear::decimal_magnitude( ++global_tests_run );
		
		const char* tests_str = gear::inscribe_unsigned_decimal( global_tests_run );
		
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
	
	void expect( bool condition, const char* ref, unsigned len )
	{
		if ( !condition )
		{
			write( STDERR_FILENO, ref, len );
		}
	
		print( condition );
	}
	
	static void log_mem( char* buffer, const char* prefix, size_t p_len, const char* mem, size_t len )
	{
		char* q = buffer;
		
		q = (char*) mempcpy( q, prefix, p_len );
		
		const char* end = mem + len;
		const char* p   = mem;
		
		while ( p < end - 1 )
		{
			gear::encode_8_bit_HEX( *p++, q );  q += 2;
			gear::encode_8_bit_HEX( *p++, q );  q += 2;
			
			*q++ = ' ';
		}
		
		if ( p < end )
		{
			gear::encode_8_bit_HEX( *p++, q );  q += 3;
		}
		
		q[ -1 ] = '\n';
		
		write( STDERR_FILENO, buffer, q - buffer );
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
			const size_t buffer_len = max( p_len, q_len );
			
			char* buffer = (char*) alloca( STRLEN( "EXPECTED: " ) + buffer_len / 2 * 5 );
		
			log_mem( buffer, STR_LEN( "EXPECTED: " ), (const char*) q, q_len );
			log_mem( buffer, STR_LEN( "RECEIVED: " ), (const char*) p, p_len );
		}
		
		return result;
	}
	
}
