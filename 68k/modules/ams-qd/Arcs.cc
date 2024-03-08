/*
	Arcs.cc
	-------
*/

#include "Arcs.hh"

// Mac OS
#ifndef __QUICKDRAW__
#include <Quickdraw.h>
#endif

// ams-common
#include "QDGlobals.hh"
#include "scoped_zone.hh"

// ams-qd
#include "OvalRgn.hh"
#include "Polygons.hh"


#pragma exceptions off


static
void clip_from_angle( RgnHandle rgn, short width, short height, short angle )
{
	Region& region = **rgn;
	
	SetRectRgn( rgn, 0, 0, width, height );
	
	switch ( angle )
	{
		case 0:
			region.rgnBBox.left = width / 2u;
			break;
		
		case 90:
			region.rgnBBox.top = height / 2u;
			break;
		
		case 180:
			region.rgnBBox.right /= 2u;
			break;
		
		case 270:
			region.rgnBBox.bottom /= 2u;
			break;
			
		default:
			goto non_rectangular;
	}
	
	return;
	
non_rectangular:
	
	short corner_set = angle ==  45 ? 0xF - 1
	                 : angle == 135 ? 0xF - 2
	                 : angle == 225 ? 0xF - 4
	                 : angle == 315 ? 0xF - 8
	                 :                0;
	
	const short n_pts = 3 + ! corner_set;
	
	const Size size = sizeof (Polygon) + n_pts * sizeof (Point);
	
	PolyHandle poly = (PolyHandle) NewHandle( size );
	
	Polygon& polygon = **poly;
	
	polygon.polySize = size;
	polygon.polyBBox = region.rgnBBox;
	
	short* p = &polygon.polyPoints[ 0 ].v;
	
	if ( corner_set )
	{
		if ( corner_set & 1 )
		{
			*p++ = 0;
			*p++ = 0;
		}
		
		if ( corner_set & 2 )
		{
			*p++ = 0;
			*p++ = width;
		}
		
		if ( corner_set & 4 )
		{
			*p++ = height;
			*p++ = width;
		}
		
		if ( corner_set & 8 )
		{
			*p++ = height;
			*p++ = 0;
		}
	}
	else
	{
		*p++ = 0;
		*p++ = 0;
		*p++ = 0;
		*p++ = width;
		*p++ = height;
		*p++ = width;
		*p++ = height;
		*p++ = 0;
		
		const short octant = angle / 45u;
		
		short factor = width;
		
		if ( octant + 1 & 0x2 )
		{
			// True for octants 1, 2, 5, and 6
			
			angle = 90 - angle;  // reciprocal slope
			
			factor = height;
		}
		
		factor /= 2u;
		
		const Fixed slope = SlopeFromAngle( angle );
		
		const short delta = factor * slope >> 16;
		
		switch ( octant )
		{
			case 0:  // 0 .. 45
				polygon.polyPoints[ 0 ].h = factor - delta;
				polygon.polyPoints[ 3 ].h = factor + delta;
				
				polygon.polyBBox.left = factor + delta;
				break;
			
			case 1:  // 45 .. 90
				polygon.polyPoints[ 0 ].v = factor - delta;
				polygon.polyPoints[ 1 ].v = factor + delta;
				
				polygon.polyBBox.top = factor + delta;
				break;
			
			case 2:  // 90 .. 135
				polygon.polyPoints[ 0 ].v = factor - delta;
				polygon.polyPoints[ 1 ].v = factor + delta;
				
				polygon.polyBBox.top = factor - delta;
				break;
			
			case 3:  // 135 .. 180
				polygon.polyPoints[ 1 ].h = factor - delta;
				polygon.polyPoints[ 2 ].h = factor + delta;
				
				polygon.polyBBox.right = factor + delta;
				break;
			
			case 4:  // 180 .. 225
				polygon.polyPoints[ 1 ].h = factor - delta;
				polygon.polyPoints[ 2 ].h = factor + delta;
				
				polygon.polyBBox.right = factor - delta;
				break;
			
			case 5:  // 225 .. 270
				polygon.polyPoints[ 2 ].v = factor + delta;
				polygon.polyPoints[ 3 ].v = factor - delta;
				
				polygon.polyBBox.bottom = factor - delta;
				break;
			
			case 6:  // 270 .. 315
				polygon.polyPoints[ 2 ].v = factor + delta;
				polygon.polyPoints[ 3 ].v = factor - delta;
				
				polygon.polyBBox.bottom = factor + delta;
				break;
			
			case 7:  // 315 .. 360 (0)
				polygon.polyPoints[ 0 ].h = factor - delta;
				polygon.polyPoints[ 3 ].h = factor + delta;
				
				polygon.polyBBox.left = factor - delta;
				break;
		}
	}
	
	polygon.polyPoints[ n_pts ] = polygon.polyPoints[ 0 ];
	
	PolyRgn( rgn, poly );
	
	DisposeHandle( (Handle) poly );
}

pascal void StdArc_patch( SInt8 verb, const Rect* r, short start, short extent )
{
	if ( extent == 0 )
	{
		return;
	}
	
	if ( extent < 0 )
	{
		start += extent;
		extent = -extent;
	}
	
	if ( start < 0 )
	{
		start = -start;
		start %= 360;
		
		start = 360 - start;
	}
	else if ( start >= 360 )
	{
		start %= 360;
	}
	
	const short width  = r->right - r->left;
	const short height = r->bottom - r->top;
	
	static RgnHandle ovalRgn = (scoped_zone(), NewRgn());
	
	if ( verb == kQDGrafVerbFrame )
	{
		verb = kQDGrafVerbPaint;
		
		const GrafPort& port = *get_thePort();
		
		const short penHeight = port.pnSize.v;
		const short penWidth  = port.pnSize.h;
		
		FramedOvalRgn( ovalRgn, width, height, penWidth, penHeight );
	}
	else
	{
		OvalRgn( ovalRgn, width, height );
	}
	
	if ( extent < 360 )
	{
		static RgnHandle clip = (scoped_zone(), NewRgn());
		
		clip_from_angle( clip, width, height, start );
		
		if ( extent != 180 )
		{
			static RgnHandle clip2 = (scoped_zone(), NewRgn());
			
			short end = start;
			
			end += extent;
			end += 180;
			end %= 360;
			
			clip_from_angle( clip2, width, height, end );
			
			if ( extent > 180 )
			{
				UnionRgn( clip, clip2, clip );
			}
			else
			{
				SectRgn( clip, clip2, clip );
			}
		}
		
		SectRgn( ovalRgn, clip, ovalRgn );
	}
	
	OffsetRgn( ovalRgn, r->left, r->top );
	
	StdRgn( verb, ovalRgn );
	
	SetEmptyRgn( ovalRgn );
}

pascal void EraseArc_patch( const Rect* rect, short start, short extent )
{
	StdArc( kQDGrafVerbErase, rect, start, extent );
}

pascal void PaintArc_patch( const Rect* rect, short start, short extent )
{
	StdArc( kQDGrafVerbPaint, rect, start, extent );
}

pascal void InvertArc_patch( const Rect* rect, short start, short extent )
{
	StdArc( kQDGrafVerbInvert, rect, start, extent );
}

pascal void FrameArc_patch( const Rect* rect, short start, short extent )
{
	StdArc( kQDGrafVerbFrame, rect, start, extent );
}

pascal void FillArc_patch( const Rect*     rect,
                           short           start,
                           short           extent,
                           const Pattern*  pattern )
{
	GrafPort& port = *get_thePort();
	
	port.fillPat = *pattern;
	
	StdArc( kQDGrafVerbFill, rect, start, extent );
}
