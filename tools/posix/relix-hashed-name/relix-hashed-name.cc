/*
	relix-hashed-name.cc
	--------------------
*/

// POSIX
#include <unistd.h>

// relix-long-filename-hash
#include "relix-long-filename-hash/hash_long_name.hh"

// plus
#include "plus/mac_utf8.hh"


#define PROGRAM  "relix-hashed-name"

#define STR_LEN( s )  "" s, (sizeof s - 1)


static
int print_hashed_name( plus::string utf8_name )
{
	plus::string mac_name = plus::mac_from_utf8( utf8_name );
	
	size_t n = mac_name.size();
	
	if ( n > 31 )
	{
		unsigned char hashed[ 32 ];
		
		relix::hash_long_name( hashed, mac_name.data(), n );
		
		mac_name = hashed;
		
		utf8_name = utf8_from_mac( mac_name );
	}
	
	write( STDOUT_FILENO, utf8_name.data(), utf8_name.size() );
	
	write( STDOUT_FILENO, STR_LEN( "\n" ) );
	
	return 0;
}

int main( int argc, char** argv )
{
	for ( int i = 1;  i < argc;  ++i )
	{
		print_hashed_name( argv[ i ] );
	}
	
	return 0;
}
