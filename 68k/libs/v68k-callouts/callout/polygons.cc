/*
	polygons.cc
	-----------
*/

#include "callout/polygons.hh"

// Standard C
#include <stdlib.h>
#include <string.h>

// iota
#include "iota/endian.hh"

// quickdraw
#include "qd/fixed.hh"
#include "qd/segments.hh"


#ifdef __MWERKS__
#pragma exceptions off
#endif


#ifndef NULL
#define NULL  0L
#endif

#define HIGH_WORD( x )  ((uint32_t) (x) >> 16)

#define EQU32( a, b )  (memcmp( &(a), &(b), sizeof (int32_t) ) == 0)


namespace v68k    {
namespace callout {

typedef unsigned char Byte;

typedef uint16_t UInt16;
typedef int32_t  Fixed;

static inline
short min( short a, short b )
{
	return b < a ? b : a;
}

static inline
short max( short a, short b )
{
	return a > b ? a : b;
}

static inline
short* rgn_extent( Byte* q )
{
	return (short*) q + 5;
}

enum
{
	rts = 0x4E75,
};

enum
{
	sizeof_region  = sizeof (short) * 5,
	sizeof_polygon = sizeof (short) * (5 + 2 * 3),  // at least three points
};

struct Point
{
	short v;
	short h;
};

struct Rect
{
	short top;
	short left;
	short bottom;
	short right;
};

static inline
long antislope( short a_h, short a_v, short b_h, short b_v )
{
	// Slope is rise/run.  This is the reciprocal of that.
	
	short rise = a_v - b_v;
	short run  = a_h - b_h;
	
	return quickdraw::fix_ratio( run, rise );
}

static inline
long antislope( Point a, Point b )
{
	return antislope( a.h, a.v, b.h, b.v );
}

static
void shrinkwrap_region_bbox( Rect& bbox, const short* extent )
{
	const short End = 0x7FFF;
	
	short left  =  32767;
	short right = -32767;
	
	const short* p = extent;
	
	while ( *p++ != End )
	{
		// v skipped above
		
		do
		{
			short h = *p++;
			
			left  = min( left,  h );
			right = max( right, h );
		}
		while ( *p != End );
		
		++p;  // skip row-ending sentinel
	}
	
	bbox.left  = left;
	bbox.right = right;
}

/*
	The logic below for generating regions from polygons keeps track of
	the bounding box of all pixels it considers.  However, sometimes some
	of the pixels near a vertex cancel each other out, and sometimes there
	are no pixels remaining just within the left or right edge of the box.
	
	Once we have the final region contents, we need to re-calculate the
	left and right edges of the bounding box for an exact fit (done above).
*/

int32_t polygon_region_callout( v68k::processor_state& s )
{
	using iota::u16_from_big;
	
	const v68k::function_code_t data_space = s.data_space();
	
	const short Region_end = 0x7FFF;
	
	const uint32_t src = s.a(0);
	const uint32_t dst = s.a(1);
	
	const uint32_t dst_size = s.d(0);
	
	const Byte* p = NULL;
	
	Byte* q = NULL;
	
	short src_size = 0;
	
	bool ok = (p = s.translate( src, sizeof_polygon, data_space, mem_read ))  &&
	          ((long) p & 0x1) == 0                                           &&
	          (src_size = u16_from_big( *(short*) p )) >= sizeof_polygon      &&
	          (p = s.translate( src, src_size, data_space, mem_read ))        &&
	          (q = s.translate( dst, dst_size, data_space, mem_write ))       &&
	          ((long) q & 0x1) == 0;
	
	if ( ! ok )
	{
		return ((long) p | (long) q) & 0x1 ? v68k::Address_error
		                                   : v68k::Bus_error;
	}
	
	const short* p2 = (const short*) p;
	
	if ( iota::is_little_endian() )
	{
		short* p = (short*) alloca( src_size );
		
		for ( int i = 0;  i < src_size / 2;  ++i )
		{
			p[ i ] = u16_from_big( p2[ i ] );
		}
		
		p2 = p;
	}
	
	const Rect& bbox = *(const Rect*) ++p2;
	
	p2 += 4;
	
	const Point* pt = (const Point*) p2;
	
	const short n_lineTo = (src_size - (sizeof_region + 4)) / 4;
	
	const bool unclosed = ! EQU32( pt[ 0 ], pt[ n_lineTo ] );
	
	if ( unclosed )
	{
		return v68k::CHK_exception;
	}
	
	const short n_unique = n_lineTo;
	
	/*
		Each scanline of a triangular region can only intersect two polygon
		edges.  With four or five sides, there can be four intersections, etc.
		But each intersection can result in two horizontal coordinates -- one
		to switch on the new area and one to cancel a prior coordinate (except
		for the first and last rows).  A complete row also contains a vertical
		coordinate and a trailing sentinel.
		
		Each point from top to bottom inclusive could have its own row in the
		region (and will, for a slope <= 1).  (Note that fenceposting applies,
		and this is points, not pixels.)
		
		Finally, there's a closing sentinel, but there's already space for two
		horizontal coordinates (at the top and bottom) that won't be used, so
		we don't need to add more.
	*/
	
	const UInt16 h_count = (n_unique & ~1) * 2;
	
	const UInt16 seg_bytes = sizeof (short) * h_count;
	
	quickdraw::segments_box p_segments( (short*) alloca( seg_bytes ) );
	quickdraw::segments_box r_segments( (short*) alloca( seg_bytes ) );
	
	short* r = rgn_extent( q );
	
	const Point** edges = (const Point**) alloca( n_unique * sizeof (void*) );
	
	const Point** ep = edges;
	
	for ( short i = 0;  i < n_unique;  ++i )
	{
		short a_v = pt++->v;
		short b_v = pt  ->v;
		
		if ( a_v != b_v )
		{
			*ep++ = pt;
		}
	}
	
	const short edge_count = ep - edges;  // non-horizontal edges only
	
	Fixed* antislopes = (Fixed*) alloca( edge_count * 4 );
	Fixed* intercepts = (Fixed*) alloca( edge_count * 4 );  // X intercepts
	
	for ( short i = 0;  i < edge_count;  ++i )
	{
		const Point* pt = edges[ i ];
		
		antislopes[ i ] = antislope( pt[ -1 ], pt[ 0 ] );
		intercepts[ i ] = 0x80000000;
	}
	
	/*
		Yes, the assignments below are correct.
		
		The region bbox might not occupy the entire poly bbox -- it could be
		narrower, if the fractional number of pixels covered by a vertex at
		the left or right edge is rounded down to zero.  So we track the
		leftmost and rightmost horizontal coordinates (i.e. min and max), and
		to do that we have to seed them with minimally left and right values.
	*/
	
	short top   = -32767;
	short left  =  32767;
	short right = -32767;
	
	short bottom = bbox.top;
	
	short v = bbox.top;
	
	while ( v < bbox.bottom )
	{
		for ( short i = 0;  i < edge_count;  ++i )
		{
			const Point a = edges[ i ][ -1 ];
			const Point b = edges[ i ][  0 ];
			
			if ( (a.v > v) == (b.v > v) )
			{
				continue;
			}
			
			Fixed  w  = antislopes[ i ];
			Fixed& xi = intercepts[ i ];
			
			short x = HIGH_WORD( xi );
			
			if ( x < bbox.left )
			{
				xi = ((a.v == v ? a.h : b.h) << 16) + w / 2;
				
				x = HIGH_WORD( xi );
			}
			
			xi += w;
			
			if ( x < left  )  left  = x;
			if ( x > right )  right = x;
			
			p_segments ^= x;
		}
		
		xor_segments( r_segments, p_segments );
		
		if ( ! r_segments.empty() )
		{
			bottom = v;
			
			accumulate_row( r, v, r_segments );
			
			r_segments.clear();
			
			if ( top < bbox.top )
			{
				top = v;
			}
		}
		
		r_segments.swap( p_segments );
		
		++v;
	}
	
	if ( ! r_segments.empty() )
	{
		bottom = v;
		
		accumulate_row( r, v, r_segments );
	}
	
	*r++ = Region_end;
	
	const short rgn_size = (char*) r - (char*) q;
	
	short* q2 = (short*) q;
	
	if ( rgn_size == sizeof_region + sizeof (short) )
	{
		*q2++ = sizeof_region;
		
		*q2++ = 0;
		*q2++ = 0;
		*q2++ = 0;
		*q2++ = 0;
	}
	else
	{
		*q2++ = rgn_size;
		
		Rect* rect = (Rect*) q2;
		
		*q2++ = top;
		*q2++ = left;
		*q2++ = bottom;
		*q2++ = right;
		
		shrinkwrap_region_bbox( *rect, q2 );
	}
	
	if ( iota::is_little_endian() )
	{
		q2 = (short*) q;
		
		for ( int i = 0;  i < rgn_size / 2;  ++i )
		{
			q2[ i ] = iota::big_u16( q2[ i ] );
		}
	}
	
	s.translate( dst, rgn_size, data_space, mem_update );
	
	return rts;
}

}  // namespace callout
}  // namespace v68k
