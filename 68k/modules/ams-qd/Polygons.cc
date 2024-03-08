/*
	Polygons.cc
	-----------
*/

#include "Polygons.hh"

// Mac OS
#ifndef __QUICKDRAW__
#include <Quickdraw.h>
#endif

// mac-glue-utils
#include "mac_glue/Memory.hh"

// ams-common
#include "callouts.hh"
#include "QDGlobals.hh"
#include "math.hh"
#include "scoped_zone.hh"


#pragma exceptions off


#define EQU32( a, b )  (*(long*) &(a) == *(long*) &(b))


void add_polygon_point( Point pt, PolyHandle poly )
{
	if ( poly[0]->polySize < sizeof (Polygon) )
	{
		const GrafPort& port = *get_thePort();
		
		poly[0]->polySize = sizeof (Polygon);
		
		poly[0]->polyPoints[ 0 ] = port.pnLoc;
	}
	
	Point* next = (Point*) ((Ptr) *poly + poly[0]->polySize);
	
	if ( (long&) pt == (long&) next[ -1 ] )
	{
		return;
	}
	
	Size size = mac::glue::GetHandleSize_raw( (Handle) poly );
	
	if ( poly[0]->polySize + sizeof (Point) > size )
	{
		SetHandleSize( (Handle) poly, 2 * size );
		
		next = (Point*) ((Ptr) *poly + poly[0]->polySize);
	}
	
	*next = pt;
	
	poly[0]->polySize += sizeof (Point);
}

void PolyRgn( RgnHandle rgn, PolyHandle poly )
{
	const Point* pt = poly[0]->polyPoints;
	
	const short n_lineTo = (poly[0]->polySize - sizeof (Polygon)) / 4;
	
	const bool unclosed = ! EQU32( pt[ 0 ], pt[ n_lineTo ] );
	
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
	
	polygon_region( *poly, *rgn, rgn_bytes );
	
	if ( unclosed )
	{
		poly[0]->polySize -= sizeof (Point);
	}
}

pascal void StdPoly_patch( signed char verb, PolyHandle poly )
{
	if ( poly[0]->polySize <= sizeof (Polygon) )
	{
		return;
	}
	
	GrafPort& port = *get_thePort();
	
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
	GrafPort& port = *get_thePort();
	
	port.fillPat = *pattern;
	
	StdPoly( kQDGrafVerbFill, poly );
}
