/*
	rle.cc
	------
*/

#include "icongen/rle.hh"

// Standard C
#include <string.h>


static
const uint8_t* find_run_end( const uint8_t* p, const uint8_t* limit )
{
	const uint8_t x = *p++;
	
	while ( p < limit  &&  *p++ == x )  continue;
	
	return --p;
}

uint8_t* rle_encode( uint8_t* dst, const uint8_t* src, const uint8_t* src_end )
{
	const uint8_t* p = src;
	const uint8_t* q = p;
	
	while ( q < src_end )
	{
		const uint8_t* limit = q + 130;
		
		if ( limit > src_end )
		{
			limit = src_end;
		}
		
		const uint8_t* end = find_run_end( q, limit );
		
		const unsigned run_length = end - q;
		
		if ( run_length >= 3 )
		{
			if ( const unsigned d = q - p )
			{
				*dst++ = d - 1;
				
				memcpy( dst, p, d );
				
				dst += d;
			}
			
			*dst++ = 125 + run_length;
			*dst++ = *q;
			
			p = q = end;
		}
		else if ( ++q - p == 128 )
		{
			*dst++ = 127;
			
			memcpy( dst, p, 128 );
			
			dst += 128;
			
			p = q;
		}
	}
	
	if ( const unsigned d = q - p )
	{
		*dst++ = d - 1;
		
		memcpy( dst, p, d );
		
		dst += d;
	}
	
	return dst;
}
