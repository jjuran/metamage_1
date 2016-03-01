/*
	sect_region.cc
	--------------
*/

#include "qd/sect_region.hh"

// Standard C
#include <stdlib.h>
#include <string.h>

// iota
#include "iota/endian.hh"

// quickdraw
#include "qd/region_detail.hh"
#include "qd/region_raster.hh"
#include "qd/region_scanner.hh"


namespace quickdraw
{
	
	typedef unsigned short uint16_t;
	
	
	static
	short min( short a, short b )
	{
		return b < a ? b : a;
	}
	
	static
	short max( short a, short b )
	{
		return a > b ? a : b;
	}
	
	static void memand( uint16_t*        dst,
	                    const uint16_t*  a,
	                    const uint16_t*  b,
	                    unsigned         size )
	{
		while ( size > 0 )
		{
			size -= 2;
			
			*dst++ = *a++ & *b++;
		}
	}
	
	void sect_rect_region( const short*  rect,
	                       const short*  bbox,
	                       const short*  extent,
	                       short*        r )
	{
		// Clip the rectangle to the region bounding box.
		
		const short top    = max( rect[ 0 ], bbox[ 0 ] );
		const short left   = max( rect[ 1 ], bbox[ 1 ] );
		const short bottom = min( rect[ 2 ], bbox[ 2 ] );
		const short right  = min( rect[ 3 ], bbox[ 3 ] );
		
		if ( top >= bottom  ||  left >= right )
		{
			*r = Region_end;
			
			return;
		}
		
		const unsigned mask_size = region_raster::mask_size( bbox ) + 1 & ~0x1;
		
		uint16_t* mask = (uint16_t*) malloc( mask_size * 2 );
		uint16_t* temp = mask + mask_size / 2;
		
		region_raster raster( bbox, extent, mask, mask_size );
		
		region_scanner scanner( r, temp, mask_size );
		
		const short rounded_left = bbox[ 1 ] & ~0xF;
		
		const int margin = -right & 0xF;
		
		const short words_before_left_margin  = (left  - rounded_left) >> 4;
		const short words_before_right_margin = (right - rounded_left - 1) >> 4;
		
		const uint16_t left_margin_mask  =  (1 << (16 - (left & 0xF))) - 1;
		const uint16_t right_margin_mask = -(1 << ((16 - right) & 0xF));
		
		uint16_t* mask_right_margin = mask + words_before_right_margin + 1;
		
		short v = top;
		
		while ( v < bottom )
		{
			raster.load_mask( v );
			
			// Trim the mask left of the intersecting rect
			
			memset( mask, '\0', words_before_left_margin * 2 );
			
			memset( mask_right_margin, '\0', (temp - mask_right_margin) * 2 );
			
			mask[ words_before_left_margin  ] &= iota::big_u16( left_margin_mask );
			mask[ words_before_right_margin ] &= iota::big_u16( right_margin_mask );
			
			if ( scanner.scan( rounded_left, v, mask, temp, margin ) )
			{
				memcpy( temp, mask, mask_size );
			}
			
			v = raster.next_v();
		}
		
		if ( v > bottom )
		{
			v = bottom;
		}
		
		scanner.finish( rounded_left, v, mask, margin );
		
		free( mask );
	}
	
	void sect_regions( const short*  bbox,
	                   const short*  a_extent,
	                   const short*  b_extent,
	                   short*        r )
	{
		const unsigned mask_size = region_raster::mask_size( bbox ) + 1 & ~0x1;
		
		uint16_t* mask_a = (uint16_t*) malloc( mask_size * 4 );
		uint16_t* mask_b = mask_a + mask_size / 2;
		uint16_t* mask_c = mask_b + mask_size / 2;
		uint16_t* mask_d = mask_c + mask_size / 2;
		
		region_raster a_raster( bbox, a_extent, mask_a, mask_size );
		region_raster b_raster( bbox, b_extent, mask_b, mask_size );
		
		region_scanner scanner( r, mask_d, mask_size );
		
		const short rounded_left = bbox[ 1 ] & ~0xF;
		
		short va = a_raster.next_v();
		short vb = b_raster.next_v();
		
		short v = precedes_in_region( va, vb ) ? vb : va;
		
		while ( va != Region_end  &&  vb != Region_end )
		{
			a_raster.load_mask( v );
			b_raster.load_mask( v );
			
			memand( mask_c, mask_a, mask_b, mask_size );
			
			if ( scanner.scan( rounded_left, v, mask_c, mask_d ) )
			{
				memcpy( mask_d, mask_c, mask_size );
			}
			
			va = a_raster.next_v();
			vb = b_raster.next_v();
			
			v = precedes_in_region( va, vb ) ? va : vb;
		}
		
		scanner.finish( rounded_left, v, mask_d );
		
		free( mask_a );
	}
	
}
