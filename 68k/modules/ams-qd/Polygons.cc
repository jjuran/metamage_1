/*
	Polygons.cc
	-----------
*/

#include "Polygons.hh"

// Mac OS
#ifndef __FIXMATH__
#include <FixMath.h>
#endif
#ifndef __QUICKDRAW__
#include <Quickdraw.h>
#endif

// Standard C
#include <stdlib.h>

// mac-glue-utils
#include "mac_glue/Memory.hh"

// quickdraw
#include "qd/region_detail.hh"
#include "qd/segments.hh"

// ams-common
#include "QDGlobals.hh"
#include "math.hh"
#include "scoped_zone.hh"

// ams-qd
#include "antislope.hh"
#include "OpenPoly.hh"
#include "segments_box.hh"


using quickdraw::Region_end;


#pragma exceptions off


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
const short* rgn_extent( const Region* rgn )
{
	return (const short*) &rgn[ 1 ];
}

static
void shrinkwrap_region_bbox( RgnHandle rgn )
{
	const short End = 0x7FFF;
	
	short left  =  32767;
	short right = -32767;
	
	const short* p = rgn_extent( *rgn );
	
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
	
	Rect& bbox = rgn[0]->rgnBBox;
	
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

void PolyRgn( RgnHandle rgn, PolyHandle poly )
{
	Point* pt = poly[0]->polyPoints;
	
	const short n_lineTo = (poly[0]->polySize - sizeof (Polygon)) / 4;
	
	const bool unclosed = *(long*) &pt[ 0 ] != *(long*) &pt[ n_lineTo ];
	
	const short n_unique = n_lineTo + unclosed;
	
	if ( n_unique < 3 )
	{
		return;
	}
	
	/*
		Go through the motions of adding the starting point in order to
		close the polygon (if necessary), but save and restore the size
		so that logically it doesn't take effect.  The benefit of doing
		so is that PolyRgn() can make use of the contiguous storage.
		
		We can't just auto-close the polygon because some applications
		rely on FramePoly() drawing an unclosed chain of line segments.
	*/
	
	if ( unclosed )
	{
		add_polygon_point( *poly[0]->polyPoints, poly );
		
		pt = poly[0]->polyPoints;
		
		poly[0]->polySize -= sizeof (Point);
	}
	
	Point** edges = (Point**) alloca( n_unique * sizeof (void*) );
	
	Point** p = edges;
	
	for ( short i = 0;  i < n_unique;  ++i )
	{
		short a_v = pt++->v;
		short b_v = pt  ->v;
		
		if ( a_v != b_v )
		{
			*p++ = pt;
		}
	}
	
	const short edge_count = p - edges;  // non-horizontal edges only
	
	Fixed* antislopes = (Fixed*) alloca( edge_count * 4 );
	Fixed* intercepts = (Fixed*) alloca( edge_count * 4 );  // X intercepts
	
	for ( short i = 0;  i < edge_count;  ++i )
	{
		Point* pt = edges[ i ];
		
		antislopes[ i ] = antislope( pt[ -1 ], pt[ 0 ] );
		intercepts[ i ] = 0x80000000;
	}
	
	const Rect& bbox = poly[0]->polyBBox;
	
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
	const UInt16 v_count = bbox.bottom - bbox.top + 1;
	
	const UInt16 seg_bytes = sizeof (short) * h_count;
	const UInt16 row_bytes = sizeof (short) * 2 + seg_bytes;
	const UInt32 rgn_bytes = sizeof (Region) + mulu_w( row_bytes, v_count );
	
	SetHandleSize( (Handle) rgn, rgn_bytes );
	
	segments_box p_segments( seg_bytes );
	segments_box r_segments( seg_bytes );
	
	short* r = (short*) (*rgn + 1);
	
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
			Point a = edges[ i ][ -1 ];
			Point b = edges[ i ][  0 ];
			
			if ( (a.v > v) == (b.v > v) )
			{
				continue;
			}
			
			Fixed  w  = antislopes[ i ];
			Fixed& xi = intercepts[ i ];
			
			short x = *(short*) &xi;
			
			if ( x < bbox.left )
			{
				xi = ((a.v == v ? a.h : b.h) << 16) + w / 2;
				
				x = *(short*) &xi;
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
	
	const short size = (char*) r - (char*) *rgn;
	
	if ( size == sizeof (Region) + sizeof (short) )
	{
		SetEmptyRgn( rgn );
	}
	else
	{
		rgn[0]->rgnSize = size;
		
		rgn[0]->rgnBBox.top    = top;
		rgn[0]->rgnBBox.left   = left;
		rgn[0]->rgnBBox.right  = right;
		rgn[0]->rgnBBox.bottom = bottom;
		
		shrinkwrap_region_bbox( rgn );
	}
}

pascal void StdPoly_patch( signed char verb, PolyHandle poly )
{
	if ( poly[0]->polySize <= sizeof (Polygon) )
	{
		return;
	}
	
	GrafPort& port = **get_addrof_thePort();
	
	if ( Handle h = port.picSave )
	{
		Size size = mac::glue::GetHandleSize_raw( h );
		
		Size new_size = size + 1 + poly[0]->polySize;
		
		if ( verb != kQDGrafVerbInvert )
		{
			new_size += 1 + sizeof (Pattern);
		}
		
		SetHandleSize( h, new_size );
		
		char* dst = *h + size;
		
		Byte pattern_op = 0;
		
		const Pattern* pattern = NULL;
		
		switch ( verb )
		{
			case kQDGrafVerbFrame:
			case kQDGrafVerbPaint:
				pattern_op = 0x09;  // PnPat
				pattern    = &port.pnPat;
				break;
			
			case kQDGrafVerbErase:
				pattern_op = 0x02;  // BkPat
				pattern    = &port.bkPat;
				break;
			
			case kQDGrafVerbFill:
				pattern_op = 0x0A;  // FillPat
				pattern    = &port.fillPat;
				break;
			
			default:
				break;
		}
		
		if ( pattern_op )
		{
			*dst++ = pattern_op;
			
			BlockMoveData( pattern, dst, sizeof (Pattern) );
			
			dst += sizeof (Pattern);
		}
		
		*dst++ = 0x70 + verb;
		
		BlockMoveData( *poly, dst, poly[0]->polySize );
	}
	
	if ( verb == kQDGrafVerbFrame )
	{
		if ( port.pnVis >= 0 )
		{
			short n = (poly[0]->polySize - sizeof (Polygon)) / 4;
			
			const Point* pt = poly[0]->polyPoints;
			
			const Point a = *pt++;  // start
			
			MoveTo( a.h, a.v );
			
			for ( short i = 0;  i < n;  ++i )
			{
				const Point b = *pt++;
				
				LineTo( b.h, b.v );
			}
		}
		
		if ( port.rgnSave == NULL )
		{
			return;
		}
	}
	
	static RgnHandle rgn = (scoped_zone(), NewRgn());
	
	PolyRgn( rgn, poly );
	
	if ( verb == kQDGrafVerbFrame  &&  port.rgnSave != NULL )
	{
		XorRgn( rgn, (RgnHandle) port.rgnSave, (RgnHandle) port.rgnSave );
	}
	else
	{
		StdRgn( verb, rgn );
	}
	
	SetEmptyRgn( rgn );
}

pascal void FramePoly_patch( PolyHandle poly )
{
	StdPoly( kQDGrafVerbFrame, poly );
}

pascal void PaintPoly_patch( PolyHandle poly )
{
	StdPoly( kQDGrafVerbPaint, poly );
}

pascal void ErasePoly_patch( PolyHandle poly )
{
	StdPoly( kQDGrafVerbErase, poly );
}

pascal void InvertPoly_patch( PolyHandle poly )
{
	StdPoly( kQDGrafVerbInvert, poly );
}

pascal void FillPoly_patch( PolyHandle poly, const Pattern* pattern )
{
	GrafPort& port = **get_addrof_thePort();
	
	port.fillPat = *pattern;
	
	StdPoly( kQDGrafVerbFill, poly );
}
