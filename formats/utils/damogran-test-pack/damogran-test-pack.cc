/*
	damogran-test-pack.cc
	---------------------
*/

// POSIX
#include <unistd.h>

// Standard C
#include <stdlib.h>
#include <string.h>

// gear
#include "gear/hexadecimal.hh"

// damogran
#include "damogran/pack.hh"


#pragma exceptions off


typedef unsigned char Byte;

static
void test_pack( char* hex, size_t n_hex )
{
	using damogran::pack;
	using damogran::pack_preflight;
	
	size_t n_unpacked = n_hex / 2;
	
	Byte* unpacked = (Byte*) hex;
	
	for ( int i = 0;  i < n_unpacked;  ++i )
	{
		unpacked[ i ] = gear::decode_8_bit_hex( hex );
		
		hex += 2;
	}
	
	long n_packed = pack_preflight( unpacked, unpacked + n_unpacked );
	
	if ( n_packed < 0 )
	{
		return;
	}
	
	if ( Byte* packed = (Byte*) malloc( n_packed * 3 + 1 ) )
	{
		Byte const* src = unpacked;
		Byte const* end = unpacked + n_unpacked;
		Byte*       dst = packed;
		
		Byte* got = pack( src, end, dst );
		
		char* hex = (char*) packed + n_packed;
		
		char* hex_end = gear::hexpcpy_lower( hex, packed, n_packed );
		
		*hex_end = '\n';
		
		write( STDOUT_FILENO, hex, n_packed * 2 + 1 );
		
		free( packed );
	}
}

int main( int argc, char** argv )
{
	char** args = argv + 1;
	
	while ( char* arg = *args++ )
	{
		test_pack( arg, strlen( arg ) );
	}
	
	return 0;
}
