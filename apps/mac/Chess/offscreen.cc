/*
	offscreen.cc
	------------
*/

#include "offscreen.hh"

// Mac OS X
#ifdef __APPLE__
#include <ApplicationServices/ApplicationServices.h>
#endif

// Mac OS
#ifndef __PROCESSES__
#include <Processes.h>
#endif

// missing-macos
#ifdef MAC_OS_X_VERSION_10_7
#include "missing/QDOffscreen.h"
#include "missing/Quickdraw.h"
#endif

// mac-qd-utils
#include "mac_qd/copy_bits.hh"
#include "mac_qd/get_portRect.hh"


#pragma exceptions off


#define CONFIG_PORTBITS  ! TARGET_API_MAC_CARBON


using mac::qd::get_portRect;


GrafPtr offscreen_port;

static BitMap back_bits;
static BitMap work_bits;

void set_offscreen_chess_board_size( short x )
{
	if ( back_bits.bounds.right == x )
	{
		return;
	}
	
	if ( back_bits.baseAddr )
	{
		DisposePtr( back_bits.baseAddr );
	}
	
	short width  = x;
	short height = x;
	
	short rowBytes = (width - 1) / 16u * 2 + 2;
	
	Size buffer_size = rowBytes * height;
	
	back_bits.baseAddr = NewPtr( buffer_size * 2 );
	
	if ( ! back_bits.baseAddr )
	{
		ExitToShell();
	}
	
	back_bits.rowBytes = rowBytes;
	
	back_bits.bounds.bottom = width;
	back_bits.bounds.right  = height;
	
	work_bits.baseAddr = back_bits.baseAddr + buffer_size;
	work_bits.rowBytes = rowBytes;
	
	work_bits.bounds.bottom = width;
	work_bits.bounds.right  = height;
	
#if CONFIG_PORTBITS
	
	if ( ! offscreen_port )
	{
		offscreen_port = (GrafPtr) NewPtr( sizeof (GrafPort) );
		
		if ( ! offscreen_port )
		{
			ExitToShell();
		}
		
		OpenPort( offscreen_port );
	}
	
	SetPortBits( &back_bits );
	
	PortSize( width, height );
	
	RectRgn( offscreen_port->visRgn, &offscreen_port->portRect );
	
#else
	
	const Rect& buffer_bounds = back_bits.bounds;
	
	if ( offscreen_port )
	{
		DisposeGWorld( offscreen_port );
		
		offscreen_port = NULL;
	}
	
	QDErr err = NewGWorld( &offscreen_port, 1, &buffer_bounds, NULL, NULL, 0 );
	
	if ( err )
	{
		ExitToShell();
	}
	
	/*
		Calling SetGWorld() here confuses the hell
		out of CarbonLib (in Classic, not the native
		Carbon.framework) in Mac OS X 10.2 and 10.3.
		
		We're not using a CLUT, so just call SetPort().
	*/
	
	SetPort( offscreen_port );
	
#endif
}

void blit( GrafPtr port )
{
	SetPort( port );
	
	mac::qd::copy_bits( offscreen_port, port );
}

void animate_icon( GrafPtr port, BitMap& icon_bits, const Rect& rect, short mode )
{
	using mac::qd::copy_bits;
	
	/*
		Animate a single frame of an icon being dragged around.
		
		We already have the game board with all units (including
		the dragged unit rendered as a silhouette) in offscreen_port.
		
		Here's the agenda:
		
		(a) copy the game board to the work area
		(b) Or/Bic the icon mask into the work area, for black/white
		(c) Xor the icon face into the work area
		
		If we're not built for the Carbon API, then we need to
		(d) copy the work area into the window.  If we are Carbon,
		then the window *is* the work area, and we rely on Aqua's
		default behavior of double-buffering its windows.
		
		NOTE:  Carbon builds running in classic Mac OS aren't double-
		buffered, which makes animation a flickery mess.  In this case,
		"live" animation is disabled and we merely draw an outline.
	*/
	
#if ! CONFIG_PORTBITS
	
	const BitMap& work_bits = *GetPortBitMapForCopyBits( port );
	
	/*
		It looks like this call to copy_bits() could have instead
		been written as `copy_bits( offscreen_port, work_bits )`,
		since work_bits is the port's bitmap -- but that isn't true
		anymore in Carbon; the "bitmap" is just an opaque token to
		pass to CopyBits(), and it's invalid to peek at the contents,
		including the bounds (which copy_bits() does internally).
	*/
	
	copy_bits( offscreen_port, port );
	
#else
	
	SetPort( offscreen_port );
	
	offscreen_port->portBits.baseAddr = work_bits.baseAddr;
	
	copy_bits( back_bits, work_bits );
	
#endif
	
	icon_bits.baseAddr += 128;
	
	copy_bits( icon_bits, work_bits, icon_bits.bounds, rect, mode );
	
	icon_bits.baseAddr -= 128;
	
	copy_bits( icon_bits, work_bits, icon_bits.bounds, rect, srcXor );
	
#if CONFIG_PORTBITS
	
	offscreen_port->portBits.baseAddr = back_bits.baseAddr;
	
	SetPort( port );
	
	copy_bits( work_bits, port );
	
#endif
}
