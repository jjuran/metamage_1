/*
	GrafPorts.cc
	------------
*/

#include "GrafPorts.hh"

// Mac OS
#ifndef __QUICKDRAW__
#include <Quickdraw.h>
#endif

// macos
#include "QDGlobals.hh"


short MemErr : 0x0220;


static inline QDGlobals& get_QDGlobals()
{
	return *(QDGlobals*) ((char*) get_addrof_thePort() - offsetof(QDGlobals, thePort));
}

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

pascal void GetPort_patch( struct GrafPort** port_ptr )
{
	*port_ptr = *get_addrof_thePort();
}

pascal void SetPort_patch( struct GrafPort* port )
{
	*get_addrof_thePort() = port;
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
	
	const short dh = left - thePort->portRect.left;
	const short dv = top  - thePort->portRect.top;
	
	thePort->portRect.bottom += dv;
	thePort->portRect.right  += dh;
	
	thePort->portRect.top  = top;
	thePort->portRect.left = left;
}
