/*
	region_raster.cc
	----------------
*/

#include "qd/region_raster.hh"

// Standard C
#include <string.h>

// quickdraw
#include "qd/region_detail.hh"


namespace quickdraw
{
	
	static inline short round_down( short n_bits )
	{
		return n_bits & ~0x07;
	}
	
	unsigned region_raster::mask_size( const short* bbox )
	{
		/*
			We can't add 7 to bbox[3] because that will overflow if it
			exceeds 32760, and it's allowed to reach 32767.  Instead,
			subtract 1 (and add 1 after shifting).  This is safe since
			the leftmost coordinate is -32767 (though the right edge has
			to be at least -32766, or the region would be empty and this
			code shouldn't be called).
		*/
		
		const short left  = ( bbox[ 1 ]      >> 3);
		const short right = ((bbox[ 3 ] - 1) >> 3) + 1;
		
		return right - left;
	}
	
	region_raster::region_raster( const short*  bbox,
	                              const short*  extent,
	                              void*         buffer,
	                              unsigned      size )
	:
		its_next( extent ),
		its_mask( (unsigned char*) buffer ),
		its_left( round_down( bbox[ 1 ] ) )
	{
		memset( its_mask, '\0', size );
		
		if ( extent == NULL )
		{
			// Rectangular region, turn on enclosed bits
			invert_mask_bits( bbox[ 1 ], bbox[ 3 ] );
		}
	}
	
	void region_raster::invert_mask_bits( short qd_begin, short qd_end )
	{
		unsigned short begin = qd_begin - its_left;
		unsigned short end   = qd_end   - its_left;
		
		const unsigned short r_begin = round_down( begin );
		const unsigned short r_end   = round_down( end   );
		
		const unsigned short d_begin = begin - r_begin;
		const unsigned short d_end   = end   - r_end;
		
		unsigned char* p = its_mask + r_begin / 8;
		
		if ( r_begin == r_end )
		{
			// Begin and end in same byte
			
			const unsigned char mask = (1 << (8 - d_begin))
			                         - (1 << (8 - d_end  ));
			
			*p ^= mask;
			
			return;
		}
		
		if ( d_begin != 0 )
		{
			const unsigned char mask = (1 << (8 - d_begin)) - 1;
			
			*p++ ^= mask;
		}
		
		const unsigned char* q = its_mask + r_end / 8;
		
		for ( ;  p < q;  ++p )
		{
			*p = ~*p;
		}
		
		if ( d_end != 0 )
		{
			const unsigned char mask = ~((1 << (8 - d_end)) - 1);
			
			*p++ ^= mask;
		}
	}
	
	void region_raster::load_mask( short v )
	{
		if ( its_next == NULL )
		{
			return;
		}
		
		while ( v >= *its_next )
		{
			const short rgn_v = *its_next++;
		
			while ( true )
			{
				short h_begin = *its_next++;
			
				if ( h_begin == Region_end )
				{
					break;
				}
			
				short h_end = *its_next++;
		
				invert_mask_bits( h_begin, h_end );
			}
		}
	}
	
}
