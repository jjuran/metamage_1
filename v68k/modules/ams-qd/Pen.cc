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

// ams-common
#include "QDGlobals.hh"


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

static
void even_dexter( RgnHandle rgn, short len )
{
	Region& region = **rgn;
	
	short* p = (short*) (*rgn + 1);
	
	short v  = region.rgnBBox.top;
	short h0 = region.rgnBBox.left;
	short h1 = h0;
	short h2 = h0;
	
	while ( --len > 0 )
	{
		++h2;
		
		*p++ = v++;
		*p++ = h0;
		*p++ = h2;
		*p++ = 0x7FFF;
		
		h0 = h1;
		h1 = h2;
	}
	
	h2 = region.rgnBBox.right;
	
	*p++ = v++;
	*p++ = h0;
	*p++ = h2;
	*p++ = 0x7FFF;
	
	*p++ = v;
	*p++ = h1;
	*p++ = h2;
	*p++ = 0x7FFF;
	*p++ = 0x7FFF;
}

static
void even_sinister( RgnHandle rgn, short len )
{
	Region& region = **rgn;
	
	short* p = (short*) (*rgn + 1);
	
	short v  = region.rgnBBox.top;
	short h0 = region.rgnBBox.right;
	short h1 = h0;
	short h2 = h0;
	
	while ( --len > 0 )
	{
		--h2;
		
		*p++ = v++;
		*p++ = h2;
		*p++ = h0;
		*p++ = 0x7FFF;
		
		h0 = h1;
		h1 = h2;
	}
	
	h2 = region.rgnBBox.left;
	
	*p++ = v++;
	*p++ = h2;
	*p++ = h0;
	*p++ = 0x7FFF;
	
	*p++ = v;
	*p++ = h2;
	*p++ = h1;
	*p++ = 0x7FFF;
	*p++ = 0x7FFF;
}

static
void shallow_dexter( RgnHandle rgn, short len, Fixed h_increment )
{
	Region& region = **rgn;
	
	short* p = (short*) (*rgn + 1);
	
	short v  = region.rgnBBox.top;
	short h0 = region.rgnBBox.left;
	short h1 = h0;
	short h2 = h0;
	
	Fixed h = h0 << 16;
	
	while ( --len > 0 )
	{
		h += h_increment;
		
		h2 = h >> 16;
		
		*p++ = v++;
		*p++ = h0;
		*p++ = h2;
		*p++ = 0x7FFF;
		
		h0 = h1;
		h1 = h2;
	}
	
	h2 = region.rgnBBox.right;
	
	*p++ = v++;
	*p++ = h0;
	*p++ = h2;
	*p++ = 0x7FFF;
	
	*p++ = v;
	*p++ = h1;
	*p++ = h2;
	*p++ = 0x7FFF;
	*p++ = 0x7FFF;
}

static
void shallow_sinister( RgnHandle rgn, short len, Fixed h_increment )
{
	Region& region = **rgn;
	
	short* p = (short*) (*rgn + 1);
	
	short v  = region.rgnBBox.top;
	short h0 = region.rgnBBox.right;
	short h1 = h0;
	short h2 = h0;
	
	Fixed h = h0 << 16;
	
	while ( --len > 0 )
	{
		h -= h_increment;
		
		h2 = h >> 16;
		
		*p++ = v++;
		*p++ = h2;
		*p++ = h0;
		*p++ = 0x7FFF;
		
		h0 = h1;
		h1 = h2;
	}
	
	h2 = region.rgnBBox.left;
	
	*p++ = v++;
	*p++ = h2;
	*p++ = h0;
	*p++ = 0x7FFF;
	
	*p++ = v;
	*p++ = h2;
	*p++ = h1;
	*p++ = 0x7FFF;
	*p++ = 0x7FFF;
}

static
void steep_dexter( RgnHandle rgn, short len, Fixed v_increment )
{
	Region& region = **rgn;
	
	short* p = (short*) (*rgn + 1);
	
	Fixed v  = region.rgnBBox.top << 16;
	short h0 = region.rgnBBox.left;
	short h1 = h0;
	short h2 = h0;
	
	while ( len-- > 0 )
	{
		*p++ = v >> 16;
		*p++ = h0;
		*p++ = ++h2;
		*p++ = 0x7FFF;
		
		v += v_increment;
		
		h0 = h1;
		h1 = h2;
	}
	
	*p++ = region.rgnBBox.bottom;
	*p++ = h0;
	*p++ = h1;
	*p++ = 0x7FFF;
	*p++ = 0x7FFF;
}

static
void steep_sinister( RgnHandle rgn, short len, Fixed v_increment )
{
	Region& region = **rgn;
	
	short* p = (short*) (*rgn + 1);
	
	Fixed v  = region.rgnBBox.top << 16;
	short h0 = region.rgnBBox.right;
	short h1 = h0;
	short h2 = h0;
	
	while ( len-- > 0 )
	{
		*p++ = v >> 16;
		*p++ = --h2;
		*p++ = h0;
		*p++ = 0x7FFF;
		
		v += v_increment;
		
		h0 = h1;
		h1 = h2;
	}
	
	*p++ = region.rgnBBox.bottom;
	*p++ = h1;
	*p++ = h0;
	*p++ = 0x7FFF;
	*p++ = 0x7FFF;
}

pascal void StdLine_patch( Point newPt )
{
	GrafPtr thePort = *get_addrof_thePort();
	
	Point pnLoc = thePort->pnLoc;
	
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
	
	line.bottom += thePort->pnSize.v;
	line.right  += thePort->pnSize.h;
	
	if ( pnLoc.v == newPt.v  ||  pnLoc.h == newPt.h )
	{
		StdRect( kQDGrafVerbPaint, &line );
		
		return;
	}
	
	static Handle handle = NewHandle( 0 );
	static Size   h_size = 0;
	
	const UInt16 descent = line.bottom - line.top;
	const UInt16 advance = line.right - line.left;
	
	const short len = min( advance, descent );
	const short pts = len + 1;
	
	const long handle_len = sizeof (Region) + 2 + pts * 8;
	
	SetHandleSize( handle, handle_len );
	
	if ( MemErr != noErr )
	{
		return;
	}
	
	h_size = handle_len;
	
	RgnHandle rgn = (RgnHandle) handle;
	
	Region& region = **rgn;
	
	region.rgnSize = handle_len;
	region.rgnBBox = line;
	
	if ( advance == descent )
	{
		if ( newPt.h >= pnLoc.h )
		{
			even_dexter( rgn, len );
		}
		else
		{
			even_sinister( rgn, len );
		}
	}
	else if ( advance > descent )
	{
		const Fixed h_increment = FixRatio( advance, descent );
		
		if ( newPt.h >= pnLoc.h )
		{
			shallow_dexter( rgn, len, h_increment );
		}
		else
		{
			shallow_sinister( rgn, len, h_increment );
		}
	}
	else
	{
		const Fixed v_increment = FixRatio( descent, advance );
		
		if ( newPt.h >= pnLoc.h )
		{
			steep_dexter( rgn, len, v_increment );
		}
		else
		{
			steep_sinister( rgn, len, v_increment );
		}
	}
	
	StdRgn( kQDGrafVerbPaint, rgn );
}

pascal void HidePen_patch()
{
	GrafPtr thePort = *get_addrof_thePort();
	
	--thePort->pnVis;
}

pascal void ShowPen_patch()
{
	GrafPtr thePort = *get_addrof_thePort();
	
	++thePort->pnVis;
}

pascal void GetPen_patch( Point* pt )
{
	GrafPtr thePort = *get_addrof_thePort();
	
	*pt = thePort->pnLoc;
}

pascal void GetPenState_patch( PenState* state )
{
	GrafPtr thePort = *get_addrof_thePort();
	
	*state = *(const PenState*) &thePort->pnLoc;
}

pascal void SetPenState_patch( const PenState* state )
{
	GrafPtr thePort = *get_addrof_thePort();
	
	*(PenState*) &thePort->pnLoc = *state;
}

pascal void PenSize_patch( short width, short height )
{
	GrafPtr thePort = *get_addrof_thePort();
	
	thePort->pnSize.v = height;
	thePort->pnSize.h = width;
}

pascal void PenMode_patch( short mode )
{
	GrafPtr thePort = *get_addrof_thePort();
	
	thePort->pnMode = mode;
}

pascal void PenPat_patch( const Pattern* pat )
{
	GrafPtr thePort = *get_addrof_thePort();
	
	thePort->pnPat = *pat;
}

pascal void PenNormal_patch()
{
	QDGlobals& qd = get_QDGlobals();
	
	GrafPtr thePort = *get_addrof_thePort();
	
	thePort->pnSize.v = 1;
	thePort->pnSize.h = 1;
	
	thePort->pnMode = patCopy;
	thePort->pnPat  = qd.black;
}

pascal void MoveTo_patch( short h, short v )
{
	GrafPtr thePort = *get_addrof_thePort();
	
	thePort->pnLoc.v = v;
	thePort->pnLoc.h = h;
}

pascal void Move_patch( short dh, short dv )
{
	GrafPtr thePort = *get_addrof_thePort();
	
	thePort->pnLoc.v += dv;
	thePort->pnLoc.h += dh;
}

pascal void LineTo_patch( short h, short v )
{
	GrafPtr thePort = *get_addrof_thePort();
	
	if ( thePort->pnVis >= 0 )
	{
		StdLine( *(const Point*) &v );
	}
	
	thePort->pnLoc.v = v;
	thePort->pnLoc.h = h;
}

pascal void Line_patch( short dh, short dv )
{
	GrafPtr thePort = *get_addrof_thePort();
	
	Point pnLoc = thePort->pnLoc;
	
	LineTo_patch( pnLoc.h + dh, pnLoc.v + dv );
}
