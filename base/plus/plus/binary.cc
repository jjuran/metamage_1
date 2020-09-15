/*
	plus/binary.cc
	--------------
*/

#include "plus/binary.hh"

// debug
#include "debug/assert.hh"


namespace plus
{
	
	string unbin( const char* data, unsigned long size, align_t align )
	{
		// Alignment must be a power of two.
		
		ASSERT( align != 0 );
		
		const unsigned mask = align - 1;
		
		ASSERT( (align ^ mask) == (align + mask) );
		
		string::size_type n_data_bytes = (size + 7) / 8;
		string::size_type n_zero_bytes = mask - (n_data_bytes + mask & mask);
		string::size_type n_bytes      = n_data_bytes + n_zero_bytes;
		
		string result;
		
		char* p = result.reset( n_bytes );
		
		while ( n_zero_bytes-- )
		{
			*p++ = 0;
		}
		
		string::size_type n = n_data_bytes;
		
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
		
		return result;
	}
	
}
