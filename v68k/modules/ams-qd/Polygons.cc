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

// quickdraw
#include "qd/region_detail.hh"
#include "qd/segments.hh"

// ams-common
#include "QDGlobals.hh"


using quickdraw::Region_end;
using quickdraw::segments_box;


static
short x_intercept( Point a, Point b, short y )
{
	short y0 = a.v;
	short y1 = b.v;
	
	if ( (y0 > y) == (y1 > y) )
	{
		return 0x8000;
	}
	
	short x0 = a.h;
	short x1 = b.h;
	
	short dy = y1 - y0;
	short dx = x1 - x0;
	
	Fixed proportional_distance = FixRatio( (y - y0) * 2 + 1, dy * 2 );
	
	return (FixMul( proportional_distance, dx << 16 ) >> 16) + x0;
}

void PolyRgn( RgnHandle rgn, PolyHandle poly )
{
	short n = (poly[0]->polySize - sizeof (Polygon)) / 4;
	
	const Rect& bbox = poly[0]->polyBBox;
	
	const short h_count = n & ~1;
	const short v_count = bbox.bottom - bbox.top + 1;
	
	const short seg_bytes = sizeof (short) * h_count;
	const short row_bytes = sizeof (short) * 2 + seg_bytes;
	const short rgn_bytes = sizeof (Region) + 2 + v_count * row_bytes;
	
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
	
	short left  =  32767;
	short right = -32767;
	
	short v = bbox.top;
	
	while ( v < bbox.bottom )
	{
		Point* pt = poly[0]->polyPoints;
		
		Point a = *pt++;  // start
		
		for ( short i = 0;  i < n;  ++i )
		{
			Point b = *pt++;
			
			short x = x_intercept( a, b, v );
			
			a = b;
			
			if ( x < bbox.left )
			{
				continue;
			}
			
			if ( x < left  )  left  = x;
			if ( x > right )  right = x;
			
			p_segments ^= x;
		}
		
		xor_segments( r_segments, p_segments );
		
		accumulate_row( r, v, r_segments );
		
		r_segments.clear();
		r_segments.swap( p_segments );
		
		++v;
	}
	
	accumulate_row( r, v, r_segments );
	
	*r++ = Region_end;
	
	rgn[0]->rgnSize = (char*) r - (char*) *rgn;
	rgn[0]->rgnBBox = bbox;
	
	rgn[0]->rgnBBox.left  = left;
	rgn[0]->rgnBBox.right = right;
}

pascal void StdPoly_patch( signed char verb, PolyHandle poly )
{
	if ( poly[0]->polySize < sizeof (Polygon) + 3 * sizeof (Point ) )
	{
		return;
	}
	
	GrafPort& port = **get_addrof_thePort();
	
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
	
	static RgnHandle rgn = NewRgn();
	
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
