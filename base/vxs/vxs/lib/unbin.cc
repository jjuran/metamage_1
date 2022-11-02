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
	
	size_t n_digits = 0;
	size_t n_digits_since_space = 0;
	
	for ( size_t i = 0;  i < size;  ++i )
	{
		if ( data[ i ] - '0' & ~1 )
		{
			if ( data[ i ] != ' ' )
			{
				return Error( "invalid binary digit" );
			}
			
			if ( n_digits_since_space % 8u != 0 )
			{
				return Error( "invalid binary digit spacing" );
			}
			
			n_digits_since_space = 0;
			
			continue;
		}
		
		++n_digits;
		++n_digits_since_space;
	}
	
	size_t n_data_bytes = (n_digits + 7) / 8;
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
	
	if ( unsigned n_loose_digits = n_digits & 7 )
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
		c = *data++;
		
		while ( c == ' ' )
		{
			c = *data++;
		}
		
		c -= '0';
		
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
