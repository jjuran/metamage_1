/*
	sect_region.cc
	--------------
*/

#include "qd/sect_region.hh"

// Standard C
#include <stdlib.h>
#include <string.h>

// Standard C++
#include <algorithm>
#include <vector>

// iota
#include "iota/endian.hh"

// quickdraw
#include "qd/region_detail.hh"
#include "qd/region_raster.hh"
#include "qd/region_scanner.hh"
#include "qd/regions.hh"


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
	
	static
	void xor_segments( std::vector< short >& segments, short coord )
	{
		if ( segments.empty()  ||  coord > segments.back() )
		{
			segments.push_back( coord );
			return;
		}
		
		typedef std::vector< short >::iterator Iter;
		
		Iter it = std::lower_bound( segments.begin(), segments.end(), coord );
		
		if ( *it == coord )
		{
			segments.erase( it );
		}
		else
		{
			segments.insert( it, coord );
		}
	}
	
	static inline
	void copy_segments( short*& r, const short* begin, const short* end )
	{
		memcpy( r, begin, (end - begin) * sizeof (short) );
		
		r += end - begin;
	}
	
	template < class Container >
	static
	void copy_segments( short*& r, const Container& container )
	{
		copy_segments( r, &*container.begin(), &*container.end() );
	}
	
	template < class Container >
	static
	void accumulate_row( short*& r, short v, const Container& segments )
	{
		if ( ! segments.empty() )
		{
			*r++ = v;
			
			copy_segments( r, segments );
			
			if ( r[ -1 ] != Region_end )
			{
				*r++ = Region_end;
			}
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
		
		std::vector< short > segments;
		
		short v, h;
		
		while ( (v = *extent++) <= top )
		{
			const short* h0 = extent;
			
			while ( *extent <= left )
			{
				++extent;
			}
			
			bool contained = (extent - h0) & 1;
			
			if ( contained )
			{
				xor_segments( segments, left );
			}
			
			while ( (h = *extent++) < right )
			{
				xor_segments( segments, h );
			}
			
			contained = (extent - h0) & 1;
			
			if ( ! contained )
			{
				xor_segments( segments, right );
			}
			
			if ( h != Region_end )
			{
				while ( *extent++ < Region_end )  continue;
			}
		}
		
		--extent;
		
		accumulate_row( r, top, segments );
		
		while ( (v = *extent++) < bottom )
		{
			*r++ = v;
			
			short* r0 = r;
			
			const short* h0 = extent;
			
			while ( *extent <= left )
			{
				++extent;
			}
			
			bool contained = (extent - h0) & 1;
			
			if ( contained )
			{
				*r++ = left;
				
				xor_segments( segments, left );
			}
			
			while ( (h = *extent++) < right )
			{
				*r++ = h;
				
				xor_segments( segments, h );
			}
			
			contained = (extent - h0) & 1;
			
			if ( ! contained )
			{
				*r++ = right;
				
				xor_segments( segments, right );
			}
			
			if ( r == r0 )
			{
				--r;
			}
			else if ( r[ -1 ] != Region_end )
			{
				*r++ = Region_end;
			}
			
			if ( h != Region_end )
			{
				while ( *extent++ < Region_end )  continue;
			}
		}
		
		accumulate_row( r, bottom, segments );
		
		*r++ = Region_end;
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
