/*
	OpenPoly.cc
	-----------
*/

#include "OpenPoly.hh"

// Mac OS
#ifndef __QUICKDRAW__
#include <Quickdraw.h>
#endif

// mac-glue-utils
#include "mac_glue/Memory.hh"

// ams-common
#include "QDGlobals.hh"


pascal PolyHandle OpenPoly_patch()
{
	GrafPort& port = **get_addrof_thePort();
	
	const Size initial_size = sizeof (Polygon) + 6 * sizeof (Point);
	
	PolyHandle poly = (PolyHandle) NewHandle( initial_size );
	
	if ( poly )
	{
		poly[0]->polySize = sizeof (Polygon) - sizeof (Point);
		
		port.polySave = (Handle) poly;
		
		HidePen();
	}
	
	return poly;
}

void add_polygon_point( Point pt, PolyHandle poly )
{
	if ( poly[0]->polySize < sizeof (Polygon) )
	{
		const GrafPort& port = **get_addrof_thePort();
		
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

pascal void ClosePoly_patch()
{
	ShowPen();
	
	GrafPort& port = **get_addrof_thePort();
	
	PolyHandle poly = (PolyHandle) port.polySave;
	
	/*
		Go through the motions of adding the starting point in order to
		close the polygon (if necessary), but save and restore the size
		so that logically it doesn't take effect.  The benefit of doing
		so is that PolyRgn() can make use of the contiguous storage.
		
		We can't just auto-close the polygon because some applications
		rely on FramePoly() drawing an unclosed chain of line segments.
	*/
	
	const short polySize = poly[0]->polySize;
	
	add_polygon_point( *poly[0]->polyPoints, poly );
	
	poly[0]->polySize = polySize;
	
	Point* pt = poly[0]->polyPoints;
	
	Point start = *pt++;
	
	short top    = start.v;
	short left   = start.h;
	short bottom = start.v;
	short right  = start.h;
	
	short n = (poly[0]->polySize - sizeof (Polygon)) / 4;
	
	while ( n-- > 0 )
	{
		if ( pt->v < top )
		{
			top = pt->v;
		}
		else if ( pt->v > bottom )
		{
			bottom = pt->v;
		}
		
		if ( pt->h < left )
		{
			left = pt->h;
		}
		else if ( pt->h > right )
		{
			right = pt->h;
		}
		
		++pt;
	}
	
	poly[0]->polyBBox.top    = top;
	poly[0]->polyBBox.left   = left;
	poly[0]->polyBBox.bottom = bottom;
	poly[0]->polyBBox.right  = right;
	
	port.polySave = NULL;
}

pascal void KillPoly_patch( PolyHandle poly )
{
	DisposeHandle( (Handle) poly );
}

pascal void OffsetPoly_patch( PolyHandle poly, short dh, short dv )
{
	Rect& bbox = poly[0]->polyBBox;
	
	const long topLeft = (long&) bbox;
	
	OffsetRect( &bbox, dh, dv );
	
	if ( topLeft == (long&) bbox )
	{
		// Either we're offsetting by (0,0), or the bbox went out of bounds.
		return;
	}
	
	Point* end = (Point*) ((char*) *poly + poly[0]->polySize);
	Point* pt  = poly[0]->polyPoints;
	
	// Offset the start point and each edge point
	
	while ( pt < end )
	{
		pt->v += dv;
		pt->h += dh;
		
		++pt;
	}
}
