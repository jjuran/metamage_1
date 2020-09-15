/*
	pack_bits.cc
	------------
*/

#include "qd/pack_bits.hh"

// Standard C
#include <string.h>


namespace quickdraw
{
	
	void unpack_bits( uint8_t const*& src, uint8_t*& dst, unsigned n_dst )
	{
		uint8_t const* p = src;
		uint8_t*       r = dst;
		
		uint8_t* end = r + n_dst;
		
		while ( r < end )
		{
			signed char c = *p++;
			
			if ( c >= 0 )
			{
				size_t n = c + 1;
				
				memcpy( r, p, n );
				
				r += n;
				p += n;
			}
			else
			{
				size_t n = 1 - c;
				
				memset( r, *p++, n );
				
				r += n;
			}
		}
		
		src = p;
		dst = r;
	}
	
}
