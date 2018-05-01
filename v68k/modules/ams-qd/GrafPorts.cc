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
#include "Rects.hh"
#include "Regions.hh"


short MemErr : 0x0220;


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
	GrafPtr thePort = *get_addrof_thePort();
	
	if ( thePort == NULL )
	{
		return;
	}
	
	thePort->portBits = *bitmap;
}

pascal void PortSize_patch( short width, short height )
{
	GrafPtr thePort = *get_addrof_thePort();
	
	if ( thePort == NULL )
	{
		return;
	}
	
	thePort->portRect.bottom = thePort->portRect.top + height;
	thePort->portRect.right = thePort->portRect.left + width;
}

pascal void MovePortTo_patch( short left, short top )
{
	GrafPtr thePort = *get_addrof_thePort();
	
	if ( thePort == NULL )
	{
		return;
	}
	
	Rect& bounds = thePort->portBits.bounds;
	
	const short dh = left + bounds.left;
	const short dv = top  + bounds.top;
	
	OffsetRect_patch( &bounds, -dh, -dv );
}

pascal void SetOrigin_patch( short h, short v )
{
	GrafPtr thePort = *get_addrof_thePort();
	
	if ( thePort == NULL )
	{
		return;
	}
	
	Rect& portRect = thePort->portRect;
	Rect& bounds   = thePort->portBits.bounds;
	
	const short dh = h - portRect.left;
	const short dv = v - portRect.top;
	
	OffsetRect_patch( &portRect, dh, dv );
	OffsetRect_patch( &bounds,   dh, dv );
	
	OffsetRgn( thePort->visRgn, dh, dv );
}

pascal void SetClip_patch( struct MacRegion** clipRgn )
{
	GrafPtr thePort = *get_addrof_thePort();
	
	if ( thePort == NULL )
	{
		return;
	}
	
	CopyRgn_patch( clipRgn, thePort->clipRgn );
}

pascal void GetClip_patch( struct MacRegion** clipRgn )
{
	GrafPtr thePort = *get_addrof_thePort();
	
	if ( thePort == NULL )
	{
		return;
	}
	
	CopyRgn_patch( thePort->clipRgn, clipRgn );
}

pascal void ClipRect_patch( const struct Rect* clipRect )
{
	GrafPtr thePort = *get_addrof_thePort();
	
	if ( thePort == NULL )
	{
		return;
	}
	
	RectRgn_patch( thePort->clipRgn, clipRect );
}

pascal void BackPat_patch( const struct Pattern* pat )
{
	GrafPtr thePort = *get_addrof_thePort();
	
	if ( thePort == NULL )
	{
		return;
	}
	
	thePort->bkPat = *pat;
}

#pragma mark -
#pragma mark Drawing in Color
#pragma mark -

pascal void ForeColor_patch( long color )
{
	GrafPtr thePort = *get_addrof_thePort();
	
	thePort->fgColor = color;
	
	WARNING = "ForeColor: unused fgColor set to ", color;
}

pascal void BackColor_patch( long color )
{
	GrafPtr thePort = *get_addrof_thePort();
	
	thePort->bkColor = color;
	
	WARNING = "BackColor: unused bkColor set to ", color;
}

#pragma mark -
#pragma mark Calculations with Points
#pragma mark -

pascal void LocalToGlobal_patch( Point* pt )
{
	GrafPtr thePort = *get_addrof_thePort();
	
	const Rect& bounds = thePort->portBits.bounds;
	
	pt->v -= bounds.top;
	pt->h -= bounds.left;
}

pascal void GlobalToLocal_patch( Point* pt )
{
	GrafPtr thePort = *get_addrof_thePort();
	
	const Rect& bounds = thePort->portBits.bounds;
	
	pt->v += bounds.top;
	pt->h += bounds.left;
}

#pragma mark -
#pragma mark Miscellaneous Routines
#pragma mark -

pascal unsigned char GetPixel_patch( short h, short v )
{
	GrafPtr thePort = *get_addrof_thePort();
	
	if ( thePort == NULL )
	{
		return false;
	}
	
	const BitMap& bits = thePort->portBits;
	const Rect& bounds = bits.bounds;
	
	v -= bounds.top;
	h -= bounds.left;
	
	Ptr addr = bits.baseAddr;
	
	addr += v * bits.rowBytes;
	addr += h / 8u;
	
	h &= 0x7;
	
	return (*addr & (1 << 7 - h)) != 0;
}
