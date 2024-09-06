/*
	pjwhash.cc
	----------
*/

// POSIX
#include <unistd.h>

// gear
#include "gear/hexadecimal.hh"

// adler32
#include "pjw-hash/pjw-hash.hh"


#pragma exceptions off


#define PROGRAM  "pjwhash"

#define STR_LEN( s )  "" s, (sizeof s - 1)


static
int pjwhash( const char* s )
{
	pjw_hash::hash_t hash = pjw_hash::function( s );
	
	char hex_buffer[ 8 ];
	
	gear::encode_32_bit_hex( hash, hex_buffer );
	
	write( STDOUT_FILENO, hex_buffer, sizeof hex_buffer );
	
	write( STDOUT_FILENO, STR_LEN( "\n" ) );
	
	return 0;
}

int main( int argc, char** argv )
{
	for ( int i = 1;  i < argc;  ++i )
	{
		pjwhash( argv[ i ] );
	}
	
	return 0;
}
