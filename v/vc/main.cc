/*
	main.cc
	-------
*/

// POSIX
#include <unistd.h>

// must
#include "must/write.h"

// plus
#include "plus/decimal.hh"
#include "plus/string/concat.hh"

// vc
#include "vc/parse.hh"


#define STR_LEN( s )  "" s, (sizeof s - 1)


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
		print( vc::parse_and_eval( expr ) );
	}
	
	return 0;
}
