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
	
	SetGWorld( offscreen_port, NULL );
	
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
	
#ifdef __APPLE__
	
	const BitMap& work_bits = *GetPortBitMapForCopyBits( port );
	
	copy_bits( offscreen_port, port );
	
#else
	
	copy_bits( offscreen_port, work_bits );
	
#endif
	
	icon_bits.baseAddr += 128;
	
	copy_bits( icon_bits, work_bits, icon_bits.bounds, rect, mode );
	
	icon_bits.baseAddr -= 128;
	
	copy_bits( icon_bits, work_bits, icon_bits.bounds, rect, srcXor );
	
#ifndef __APPLE__
	
	copy_bits( work_bits, port );
	
#endif
}
