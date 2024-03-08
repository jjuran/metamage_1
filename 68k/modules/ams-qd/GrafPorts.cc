/*
	GrafPorts.cc
	------------
*/

#include "GrafPorts.hh"

// log-of-war
#include "logofwar/report.hh"

// ams-common
#include "QDGlobals.hh"

// ams-qd
#include "color.hh"
#include "Rects.hh"
#include "Regions.hh"


#pragma exceptions off


short MemErr : 0x0220;

char* ScrnBase : 0x0824;
Rect  CrsrRect : 0x083C;
char* CrsrSave : 0x088C;


#pragma mark -
#pragma mark GrafPort Routines
#pragma mark -

pascal void OpenPort_patch( struct GrafPort* port )
{
	port->visRgn  = NewRgn();  if ( port->visRgn  == NULL )  goto fail_visRgn;
	port->clipRgn = NewRgn();  if ( port->clipRgn == NULL )  goto fail_clipRgn;
	
	InitPort( port );
	
	return;
	
fail_clipRgn:
	DisposeRgn( port->visRgn );
	
	port->visRgn = NULL;

fail_visRgn:
	port->device = MemErr;
}

pascal void InitPort_patch( struct GrafPort* port )
{
	QDGlobals& qd = get_QDGlobals();
	
	port->device   = 0;
	port->portBits = qd.screenBits;
	port->portRect = qd.screenBits.bounds;
	
	RectRgn( port->visRgn, &qd.screenBits.bounds );
	
	SetRectRgn( port->clipRgn, -32767, -32767, 32767, 32767 );
	
	port->bkPat   = qd.white;
	port->fillPat = qd.black;
	
	port->pnLoc.v = 0;
	port->pnLoc.h = 0;
	port->pnSize.v = 1;
	port->pnSize.h = 1;
	
	port->pnMode     = patCopy;
	port->pnPat      = qd.black;
	port->pnVis      = 0;
	port->txFont     = 0;
	port->txFace     = 0;
	port->txMode     = srcOr;
	port->txSize     = 0;
	port->spExtra    = 0;
	port->fgColor    = blackColor;
	port->bkColor    = whiteColor;
	port->colrBit    = 0;
	port->patStretch = 0;
	port->picSave    = NULL;
	port->rgnSave    = NULL;
	port->polySave   = NULL;
	port->grafProcs  = NULL;
	
	qd.thePort = port;
}

pascal void ClosePort_patch( struct GrafPort* port )
{
	DisposeRgn( port->visRgn  );
	DisposeRgn( port->clipRgn );
}

pascal void SetPort_patch( struct GrafPort* port )
{
	*get_addrof_thePort() = port;
}

pascal void GetPort_patch( struct GrafPort** port_ptr )
{
	*port_ptr = *get_addrof_thePort();
}

pascal void SetPortBits_patch( const BitMap* bitmap )
{
	GrafPtr thePort = get_thePort();
	
	thePort->portBits = *bitmap;
}

pascal void PortSize_patch( short width, short height )
{
	GrafPtr thePort = get_thePort();
	
	thePort->portRect.bottom = thePort->portRect.top + height;
	thePort->portRect.right = thePort->portRect.left + width;
}

pascal void MovePortTo_patch( short left, short top )
{
	GrafPtr thePort = get_thePort();
	
	Rect& bounds = thePort->portBits.bounds;
	
	const short dh = left + bounds.left;
	const short dv = top  + bounds.top;
	
	OffsetRect( &bounds, -dh, -dv );
}

pascal void SetOrigin_patch( short h, short v )
{
	GrafPtr thePort = get_thePort();
	
	Rect& portRect = thePort->portRect;
	Rect& bounds   = thePort->portBits.bounds;
	
	const short dh = h - portRect.left;
	const short dv = v - portRect.top;
	
	OffsetRect( &portRect, dh, dv );
	OffsetRect( &bounds,   dh, dv );
	
	OffsetRgn( thePort->visRgn, dh, dv );
}

pascal void SetClip_patch( struct MacRegion** clipRgn )
{
	GrafPtr thePort = get_thePort();
	
	CopyRgn( clipRgn, thePort->clipRgn );
}

pascal void GetClip_patch( struct MacRegion** clipRgn )
{
	GrafPtr thePort = get_thePort();
	
	CopyRgn( thePort->clipRgn, clipRgn );
}

pascal void ClipRect_patch( const struct Rect* clipRect )
{
	GrafPtr thePort = get_thePort();
	
	RectRgn( thePort->clipRgn, clipRect );
}

pascal void BackPat_patch( const struct Pattern* pat )
{
	GrafPtr thePort = get_thePort();
	
	thePort->bkPat = *pat;
}

#pragma mark -
#pragma mark Drawing in Color
#pragma mark -

pascal void ForeColor_patch( long color )
{
	GrafPtr thePort = get_thePort();
	
	if ( color == thePort->fgColor  &&  color == blackColor )
	{
		NOTICE = "ForeColor: redundantly set to black";
	}
	
	if ( is_nongray( color ) )
	{
		WARNING = "ForeColor: non-gray color ", color, " will render as black";
	}
	
	thePort->fgColor = color;
}

pascal void BackColor_patch( long color )
{
	GrafPtr thePort = get_thePort();
	
	if ( color == thePort->bkColor  &&  color == whiteColor )
	{
		NOTICE = "BackColor: redundantly set to white";
	}
	
	if ( is_nongray( color ) )
	{
		WARNING = "BackColor: non-gray color ", color, " will render as black";
	}
	
	thePort->bkColor = color;
}

#pragma mark -
#pragma mark Calculations with Points
#pragma mark -

pascal void LocalToGlobal_patch( Point* pt )
{
	if ( GrafPtr thePort = *get_addrof_thePort() )
	{
		const Rect& bounds = thePort->portBits.bounds;
		
		pt->v -= bounds.top;
		pt->h -= bounds.left;
	}
}

pascal void GlobalToLocal_patch( Point* pt )
{
	if ( GrafPtr thePort = *get_addrof_thePort() )
	{
		const Rect& bounds = thePort->portBits.bounds;
		
		pt->v += bounds.top;
		pt->h += bounds.left;
	}
}

#pragma mark -
#pragma mark Miscellaneous Routines
#pragma mark -

static inline
bool onscreen( const BitMap& bitmap )
{
	return bitmap.baseAddr == ScrnBase;
}

static inline
bool intersects_cursor( const BitMap& bitmap, Point pt )
{
	return onscreen( bitmap )  &&  PtInRect( pt, &CrsrRect );
}

pascal unsigned char GetPixel_patch( short h, short v )
{
	GrafPtr thePort = get_thePort();
	
	const BitMap& bits = thePort->portBits;
	const Rect& bounds = bits.bounds;
	
	v -= bounds.top;
	h -= bounds.left;
	
	Ptr   addr     = bits.baseAddr;
	short rowBytes = bits.rowBytes;
	
	/*
		Missile calls GetPixel() to hit-test incoming warheads against the
		detonations of the player's defensive munitions.  It checks for two
		adjacent black pixels, which will never match a warhead's contrail
		(which is drawn with a checkerboard pattern).  Testing on a real Mac
		reveals that it doesn't match the cursor, either, so it's wrong to
		sample an area of the screen that intersects the cursor.
		
		Rather than spend time hiding and showing the cursor (and generating
		a false-positive screen update), check to see if the point is within
		CrsrRect, and if so, query the cursor-saved-bits buffer directly.
	*/
	
	if ( intersects_cursor( bits, *(Point*) &v ) )
	{
		v -= CrsrRect.top;
		h -= CrsrRect.left;
		
		addr     = CrsrSave;
		rowBytes = sizeof (UInt32);
	}
	
	addr += v * rowBytes;
	addr += h / 8u;
	
	h &= 0x7;
	
	return (*addr & (1 << 7 - h)) != 0;
}
