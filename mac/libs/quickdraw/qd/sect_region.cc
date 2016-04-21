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


namespace quickdraw
{
	
	typedef std::vector< short > segments_box;
	
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
	
	static
	void xor_segments( segments_box& segments, short coord )
	{
		typedef segments_box::iterator Iter;
		
		Iter end = segments.end();
		
		Iter it = std::lower_bound( segments.begin(), end, coord );
		
		if ( it != end  &&  *it == coord )
		{
			segments.erase( it );
		}
		else
		{
			segments.insert( it, coord );
		}
	}
	
	static
	void xor_segments( segments_box&        a,
	                   segments_box const&  b )
	{
		typedef segments_box::const_iterator Iter;
		
		Iter it = b.begin();
		
		while ( it != b.end() )
		{
			xor_segments( a, *it++ );
		}
	}
	
	static
	void and_segments( segments_box const&  a,
	                   segments_box const&  b,
	                   segments_box&        c )
	{
		c.clear();
		
		if ( a.empty()  ||  b.empty() )
		{
			return;
		}
		
		typedef segments_box::const_iterator Iter;
		
		bool in_a = false;
		bool in_b = false;
		
		Iter it_a = a.begin();
		Iter it_b = b.begin();
		
		short x = *it_a++;
		short y = *it_b++;
		
		bool was_in = false;
		
		short min;
		
		while ( true )
		{
			if ( x <= y )
			{
				in_a = ! in_a;
				
				min = x;
			}
			
			if ( y <= x )
			{
				in_b = ! in_b;
				
				min = y;
			}
			
			bool is_in = in_a  &&  in_b;
			
			if ( is_in != was_in )
			{
				c.push_back( min );
			}
			
			was_in = is_in;
			
			if ( x == min )
			{
				if ( it_a == a.end() )
				{
					break;
				}
				
				x = *it_a++;
			}
			
			if ( y == min )
			{
				if ( it_b == b.end() )
				{
					break;
				}
				
				y = *it_b++;
			}
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
	                       unsigned      max_bytes,
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
	                   unsigned      a_max_bytes,
	                   const short*  b_extent,
	                   unsigned      b_max_bytes,
	                   short*        r )
	{
		std::vector< short > a_segments;
		std::vector< short > b_segments;
		std::vector< short > c_segments;
		std::vector< short > r_segments;
		
		short va = *a_extent++;
		short vb = *b_extent++;
		
		while ( va != Region_end  &&  vb != Region_end )
		{
			short v = va;
			
			if ( ! precedes_in_region( vb, va ) )
			{
				short h;
				
				while ( (h = *a_extent++) != Region_end )
				{
					xor_segments( a_segments, h );
				}
				
				va = *a_extent++;
			}
			
			if ( ! precedes_in_region( v, vb ) )
			{
				v = vb;
				
				short h;
				
				while ( (h = *b_extent++) != Region_end )
				{
					xor_segments( b_segments, h );
				}
				
				vb = *b_extent++;
			}
			
			and_segments( a_segments, b_segments, c_segments );
			xor_segments( r_segments, c_segments );
			
			accumulate_row( r, v, r_segments );
			
			r_segments.swap( c_segments );
		}
		
		*r++ = Region_end;
	}
	
}
