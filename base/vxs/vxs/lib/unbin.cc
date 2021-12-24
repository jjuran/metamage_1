/*
	unbin.cc
	--------
*/

#include "vxs/lib/unbin.hh"

// debug
#include "debug/assert.hh"


namespace vxo
{

Expected_String unbin( const char* data, size_t size, align_t align )
{
	// Alignment must be a power of two.
	
	ASSERT( align != 0 );
	
	const unsigned mask = align - 1;
	
	ASSERT( (align ^ mask) == (align + mask) );
	
	for ( size_t i = 0;  i < size;  ++i )
	{
		if ( data[ i ] - '0' & ~1 )
		{
			return Error( "invalid binary digit" );
		}
	}
	
	size_t n_data_bytes = (size + 7) / 8;
	size_t n_zero_bytes = mask - (n_data_bytes + mask & mask);
	size_t n_bytes      = n_data_bytes + n_zero_bytes;
	
	plus::string result;
	
	char* p = result.reset( n_bytes );
	
	while ( n_zero_bytes-- )
	{
		*p++ = 0;
	}
	
	size_t n = n_data_bytes;
	
	unsigned char c;
	
	if ( unsigned n_loose_digits = size & 7 )
	{
		c = 0;
		
		while ( n_loose_digits-- )
		{
			c = *data++ - '0' | (c << 1);
		}
		
		*p++ = c;
		
		--n;
	}
	
	for ( ;  n > 0;  --n )
	{
		c = *data++ - '0';
		c = *data++ - '0' | (c << 1);
		c = *data++ - '0' | (c << 1);
		c = *data++ - '0' | (c << 1);
		c = *data++ - '0' | (c << 1);
		c = *data++ - '0' | (c << 1);
		c = *data++ - '0' | (c << 1);
		c = *data++ - '0' | (c << 1);
		
		*p++ = c;
	}
	
	return String( result );
}

}
