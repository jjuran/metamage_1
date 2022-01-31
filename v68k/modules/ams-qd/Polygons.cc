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

// quickdraw
#include "qd/region_detail.hh"
#include "qd/segments.hh"

// ams-common
#include "QDGlobals.hh"
#include "scoped_zone.hh"

// ams-qd
#include "antislope.hh"
#include "segments_box.hh"


using quickdraw::Region_end;


void PolyRgn( RgnHandle rgn, PolyHandle poly )
{
	short n = (poly[0]->polySize - sizeof (Polygon)) / 4;
	
	Point** edges = (Point**) alloca( n * 4 );
	
	Point** p = edges;
	
	Point* pt = poly[0]->polyPoints;
	
	for ( short i = 0;  i < n;  ++i )
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
	
	const short h_count = (n & ~1) * 2;
	const short v_count = bbox.bottom - bbox.top + 1;
	
	const short seg_bytes = sizeof (short) * h_count;
	const short row_bytes = sizeof (short) * 2 + seg_bytes;
	const short rgn_bytes = sizeof (Region) + v_count * row_bytes;
	
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
		return;
	}
	
	rgn[0]->rgnSize = size;
	
	rgn[0]->rgnBBox.top    = top;
	rgn[0]->rgnBBox.left   = left;
	rgn[0]->rgnBBox.right  = right;
	rgn[0]->rgnBBox.bottom = bottom;
}

pascal void StdPoly_patch( signed char verb, PolyHandle poly )
{
	if ( poly[0]->polySize < sizeof (Polygon) + 3 * sizeof (Point ) )
	{
		return;
	}
	
	GrafPort& port = **get_addrof_thePort();
	
	if ( Handle h = port.picSave )
	{
		Size size = GetHandleSize( h );
		
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
