/*
	pack_bits.cc
	------------
*/

#include "qd/pack_bits.hh"

// Standard C
#include <string.h>

// more-libc
#include "more/string.h"


namespace quickdraw
{
	
	void pack_bits( uint8_t const*& src, unsigned n_src, uint8_t*& dst )
	{
		uint8_t const* p = src;
		uint8_t*       r = dst;
		
		unsigned n = n_src;
		
		while ( n > 127 )
		{
			*r++ = 127;
			
			r = (uint8_t*) mempcpy( r, p, 128 );
			
			p += 128;
			n -= 128;
		}
		
		if ( n )
		{
			*r++ = n - 1;
			
			r = (uint8_t*) mempcpy( r, p, n );
			
			p += n;
		}
		
		src = p;
		dst = r;
	}
	
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
				
				r = (uint8_t*) mempcpy( r, p, n );
				
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
