/*
	Pen.cc
	------
*/

#include "Pen.hh"

// Mac OS
#ifndef __FIXMATH__
#include <FixMath.h>
#endif

// iota
#include "iota/swap.hh"

// quickdraw
#include "qd/convex_region_generator.hh"

// ams-common
#include "QDGlobals.hh"

// ams-qd
#include "antislope.hh"
#include "GrafProcs.hh"
#include "Polygons.hh"
#include "Regions.hh"


using quickdraw::convex_region_generator;


short MemErr : 0x0220;

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
Fixed min( Fixed a, Fixed b )
{
	return b < a ? b : a;
}

static inline
Fixed max( Fixed a, Fixed b )
{
	return a > b ? a : b;
}

static
short* even_dexter( RgnHandle rgn, Point penSize )
{
	short* p = (short*) (*rgn + 1);
	
	convex_region_generator generator( p );
	
	const Rect& bbox = rgn[0]->rgnBBox;
	
	const short v0 = bbox.top;
	const short vn = bbox.bottom;
	
	const short vertex_leading = v0 + penSize.v;
	const short vertex_closing = vn - penSize.v + 1;
	
	const short v1 = min( vertex_leading, vertex_closing );
	const short v2 = max( vertex_leading, vertex_closing );
	
	short v = v0;
	
	short h0 = bbox.left;
	short h1 = h0 + penSize.h;
	
	generator.start( v, h0, h1 );
	
	while ( ++v < v1 )
	{
		generator.extend_right( v, ++h1 );
	}
	
	while ( v < vertex_closing )
	{
		generator.tack_right( v++, ++h0, ++h1 );
	}
	
	v = v2;
	
	while ( v < vn )
	{
		generator.condense_right( v++, ++h0 );
	}
	
	return generator.finish( v );
}

static
short* even_sinister( RgnHandle rgn, Point penSize )
{
	short* p = (short*) (*rgn + 1);
	
	convex_region_generator generator( p );
	
	const Rect& bbox = rgn[0]->rgnBBox;
	
	const short v0 = bbox.top;
	const short vn = bbox.bottom;
	
	const short vertex_leading = v0 + penSize.v;
	const short vertex_closing = vn - penSize.v + 1;
	
	const short v1 = min( vertex_leading, vertex_closing );
	const short v2 = max( vertex_leading, vertex_closing );
	
	short v = v0;
	
	short h0 = bbox.right;
	short h1 = h0 - penSize.h;
	
	generator.start( v, h1, h0 );
	
	while ( ++v < v1 )
	{
		generator.extend_left( v, --h1 );
	}
	
	while ( v < vertex_closing )
	{
		generator.tack_left( v++, --h1, --h0 );
	}
	
	v = v2;
	
	while ( v < vn )
	{
		generator.condense_left( v++, --h0 );
	}
	
	return generator.finish( v );
}

static
short* shallow_dexter( RgnHandle rgn, Point penSize, Fixed h_increment )
{
	short* p = (short*) (*rgn + 1);
	
	convex_region_generator generator( p );
	
	const Rect& bbox = rgn[0]->rgnBBox;
	
	const short v0 = bbox.top;
	const short vn = bbox.bottom;
	
	const short vertex_leading = v0 + penSize.v;
	const short vertex_closing = vn - penSize.v + 1;
	
	const short v1 = min( vertex_leading, vertex_closing );
	const short v2 = max( vertex_leading, vertex_closing );
	
	short v = v0;
	
	const short h0    = bbox.left;
	const Fixed h_max = bbox.right << 16;
	
	Fixed h1 = (h0                 << 16) + h_increment / 2u + 0x8000;
	Fixed h2 = (h0 + penSize.h - 1 << 16) + h_increment / 2u + 0x8000;
	
	generator.start( v, h0, h2 >> 16 );
	
	while ( ++v < v1 )
	{
		h2 = min( h2 + h_increment, h_max );
		
		generator.extend_right( v, h2 >> 16 );
	}
	
	while ( v < vertex_closing )
	{
		h2 = min( h2 + h_increment, h_max );
		
		generator.tack_right( v++, h1 >> 16, h2 >> 16 );
		
		h1 += h_increment;
	}
	
	v = v2;
	
	while ( v < vn )
	{
		generator.condense_right( v++, h1 >> 16 );
		
		h1 += h_increment;
	}
	
	return generator.finish( v );
}

static
short* shallow_sinister( RgnHandle rgn, Point penSize, Fixed h_increment )
{
	short* p = (short*) (*rgn + 1);
	
	convex_region_generator generator( p );
	
	const Rect& bbox = rgn[0]->rgnBBox;
	
	const short v0 = bbox.top;
	const short vn = bbox.bottom;
	
	const short vertex_leading = v0 + penSize.v;
	const short vertex_closing = vn - penSize.v + 1;
	
	const short v1 = min( vertex_leading, vertex_closing );
	const short v2 = max( vertex_leading, vertex_closing );
	
	short v = v0;
	
	const short h0    = bbox.right;
	const Fixed h_min = bbox.left << 16;
	
	Fixed h1 = (h0                 << 16) + -(-h_increment / 2u) + 0x8000;
	Fixed h2 = (h0 - penSize.h + 1 << 16) + -(-h_increment / 2u) + 0x8000;
	
	generator.start( v, h2 >> 16, h0 );
	
	while ( ++v < v1 )
	{
		h2 = max( h2 + h_increment, h_min );
		
		generator.extend_left( v, h2 >> 16 );
	}
	
	while ( v < vertex_closing )
	{
		h2 = max( h2 + h_increment, h_min );
		
		generator.tack_left( v++, h2 >> 16, h1 >> 16 );
		
		h1 += h_increment;
	}
	
	v = v2;
	
	while ( v < vn )
	{
		generator.condense_left( v++, h1 >> 16 );
		
		h1 += h_increment;
	}
	
	return generator.finish( v );
}

static
short* steep_dexter( RgnHandle rgn, Point penSize, Fixed h_increment )
{
	short* p = (short*) (*rgn + 1);
	
	convex_region_generator generator( p );
	
	const Rect& bbox = rgn[0]->rgnBBox;
	
	const short v0 = bbox.top;
	const short vn = bbox.bottom;
	
	const short vertex_leading = v0 + penSize.v;
	const short vertex_closing = vn - penSize.v + 1;
	
	const short v1 = min( vertex_leading, vertex_closing );
	const short v2 = max( vertex_leading, vertex_closing );
	
	short v = v0;
	
	const short h0 = bbox.left;
	const short hn = bbox.right;
	
	Fixed h1 = (h0             << 16) + 0x8000;
	Fixed h2 = (h0 + penSize.h << 16) + 0x8000 + h_increment / 2u;
	
	generator.start( v, h0, h2 >> 16 );
	
	while ( ++v < v1 )
	{
		h2 += h_increment;
		
		generator.move_right( v, h0, h2 >> 16 );
	}
	
	while ( v < vertex_closing )
	{
		h1 += h_increment;
		h2 += h_increment;
		
		generator.move_right( v++, h1 >> 16, h2 >> 16 );
	}
	
	v = v2;
	
	while ( v < vn )
	{
		h1 += h_increment;
		
		generator.move_right( v++, h1 >> 16, hn );
	}
	
	return generator.finish( v );
}

static
short* steep_sinister( RgnHandle rgn, Point penSize, Fixed h_increment )
{
	short* p = (short*) (*rgn + 1);
	
	convex_region_generator generator( p );
	
	const Rect& bbox = rgn[0]->rgnBBox;
	
	const short v0 = bbox.top;
	const short vn = bbox.bottom;
	
	const short vertex_leading = v0 + penSize.v;
	const short vertex_closing = vn - penSize.v + 1;
	
	const short v1 = min( vertex_leading, vertex_closing );
	const short v2 = max( vertex_leading, vertex_closing );
	
	short v = v0;
	
	const short h0 = bbox.right;
	const short hn = bbox.left;
	
	Fixed h1 = (h0             << 16) + 0x8000 + -(-h_increment / 2u);
	Fixed h2 = (h0 - penSize.h << 16) + 0x8000 + -(-h_increment / 2u);
	
	generator.start( v, h2 >> 16, h0 );
	
	while ( ++v < v1 )
	{
		h2 += h_increment;
		
		generator.move_left( v, h2 >> 16, h0 );
	}
	
	while ( v < vertex_closing )
	{
		h1 += h_increment;
		h2 += h_increment;
		
		generator.move_left( v++, h2 >> 16, h1 >> 16 );
	}
	
	v = v2;
	
	while ( v < vn )
	{
		h1 += h_increment;
		
		generator.move_left( v++, hn, h1 >> 16 );
	}
	
	return generator.finish( v );
}

pascal void StdLine_patch( Point newPt )
{
	const GrafPort& port = *get_thePort();
	
	if ( port.polySave )
	{
		add_polygon_point( newPt, (PolyHandle) port.polySave );
	}
	
	if ( port.pnVis < 0 )
	{
		return;
	}
	
	Point pnLoc = port.pnLoc;
	
	if ( pnLoc.v > newPt.v )
	{
		using iota::swap;
		
		swap( pnLoc, newPt );
	}
	
	Rect line;
	
	line.top    = min( pnLoc.v, newPt.v );
	line.left   = min( pnLoc.h, newPt.h );
	line.bottom = max( pnLoc.v, newPt.v );
	line.right  = max( pnLoc.h, newPt.h );
	
	const short descent = line.bottom - line.top;
	const short advance = line.right - line.left;
	
	line.bottom += port.pnSize.v;
	line.right  += port.pnSize.h;
	
	if ( pnLoc.v == newPt.v  ||  pnLoc.h == newPt.h )
	{
		StdRect( kQDGrafVerbPaint, &line );
		
		return;
	}
	
	static Handle handle = NewHandle( 0 );
	
	const short pts = line.bottom - line.top + 1;
	
	const long handle_len = sizeof (Region) + 2 + pts * 6 * sizeof (short);
	
	SetHandleSize( handle, handle_len );
	
	if ( MemErr != noErr )
	{
		return;
	}
	
	RgnHandle rgn = (RgnHandle) handle;
	
	Region& region = **rgn;
	
	region.rgnBBox = line;
	
	short* end;
	
	if ( advance == descent )
	{
		if ( newPt.h >= pnLoc.h )
		{
			end = even_dexter( rgn, port.pnSize );
		}
		else
		{
			end = even_sinister( rgn, port.pnSize );
		}
	}
	else if ( advance > descent )
	{
		const Fixed h_increment = antislope( newPt, pnLoc );
		
		if ( newPt.h >= pnLoc.h )
		{
			end = shallow_dexter( rgn, port.pnSize, h_increment );
		}
		else
		{
			end = shallow_sinister( rgn, port.pnSize, h_increment );
		}
	}
	else
	{
		const Fixed h_increment = antislope( newPt, pnLoc );
		
		if ( newPt.h >= pnLoc.h )
		{
			end = steep_dexter( rgn, port.pnSize, h_increment );
		}
		else
		{
			end = steep_sinister( rgn, port.pnSize, h_increment );
		}
	}
	
	rgn[0]->rgnSize = (char*) end - (char*) *rgn;
	
	StdRgn_patch( kQDGrafVerbPaint, rgn );
}

pascal void HidePen_patch()
{
	GrafPort& port = *get_thePort();
	
	--port.pnVis;
}

pascal void ShowPen_patch()
{
	GrafPort& port = *get_thePort();
	
	++port.pnVis;
}

pascal void GetPen_patch( Point* pt )
{
	const GrafPort& port = *get_thePort();
	
	*pt = port.pnLoc;
}

pascal void GetPenState_patch( PenState* state )
{
	const GrafPort& port = *get_thePort();
	
	*state = *(const PenState*) &port.pnLoc;
}

pascal void SetPenState_patch( const PenState* state )
{
	GrafPort& port = *get_thePort();
	
	*(PenState*) &port.pnLoc = *state;
}

pascal void PenSize_patch( short width, short height )
{
	GrafPort& port = *get_thePort();
	
	port.pnSize.v = height;
	port.pnSize.h = width;
}

pascal void PenMode_patch( short mode )
{
	GrafPort& port = *get_thePort();
	
	port.pnMode = mode;
}

pascal void PenPat_patch( const Pattern* pat )
{
	GrafPort& port = *get_thePort();
	
	port.pnPat = *pat;
}

pascal void PenNormal_patch()
{
	QDGlobals& qd = get_QDGlobals();
	
	GrafPort& port = *get_thePort();
	
	port.pnSize.v = 1;
	port.pnSize.h = 1;
	
	port.pnMode = patCopy;
	port.pnPat  = qd.black;
}

pascal void MoveTo_patch( short h, short v )
{
	GrafPort& port = *get_thePort();
	
	if ( port.rgnSave  &&  port.polySave )
	{
		PolyHandle poly = (PolyHandle) port.polySave;
		
		const short n_pts = (poly[0]->polySize - sizeof (Polygon)) / 4u;
		
		if ( n_pts > 0 )
		{
			ClosePoly();
			
			if ( n_pts >= 3 )
			{
				FramePoly( poly );
			}
			
			KillPoly( poly );
			
			OpenPoly();
		}
	}
	
	port.pnLoc.v = v;
	port.pnLoc.h = h;
}

pascal void Move_patch( short dh, short dv )
{
	const GrafPort& port = *get_thePort();
	
	Point pnLoc = port.pnLoc;
	
	MoveTo_patch( pnLoc.h + dh, pnLoc.v + dv );
}

pascal void LineTo_patch( short h, short v )
{
	GrafPort& port = *get_thePort();
	
	CHECK_CUSTOM_GRAFPROC( port, lineProc );
	
	GRAFPROC_LINE( port )( *(const Point*) &v );
	
	port.pnLoc.v = v;
	port.pnLoc.h = h;
}

pascal void Line_patch( short dh, short dv )
{
	const GrafPort& port = *get_thePort();
	
	Point pnLoc = port.pnLoc;
	
	LineTo_patch( pnLoc.h + dh, pnLoc.v + dv );
}
