/*
	main.cc
	-------
*/

// POSIX
#include <unistd.h>

// Standard C++
#include <new>

// must
#include "must/write.h"

// plus
#include "plus/decimal.hh"
#include "plus/string/concat.hh"

// vc
#include "vc/parse.hh"


#define STR_LEN( s )  "" s, (sizeof s - 1)

#define FAIL( s )  fail( STR_LEN( "ERROR: " s "\n" ) )


static int fail( const char* msg, unsigned len )
{
	must_write( STDERR_FILENO, msg, len );
	
	return 1;
}

static void print( const plus::integer& i )
{
	plus::string s = encode_decimal( i ) + "\n";
	
	must_write( STDOUT_FILENO, s.data(), s.size() );
}


int main( int argc, char** argv )
{
	if ( argc < 2 )
	{
		must_write( STDERR_FILENO, STR_LEN( "Argument required\n" ) );
		
		return 2;
	}
	
	++argv;
	
	while ( const char* expr = *argv++ )
	{
		try
		{
			print( vc::parse_and_eval( expr ) );
		}
		catch ( const std::bad_alloc& )
		{
			return FAIL( "Out of memory!" );
		}
		catch ( const plus::ibox::limb_count_overflow& )
		{
			return FAIL( "Max bigint size exceeded" );
		}
	}
	
	return 0;
}
