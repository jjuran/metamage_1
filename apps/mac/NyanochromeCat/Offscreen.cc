/*
	Offscreen.cc
	------------
*/

#include "Offscreen.hh"

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
#ifndef MISSING_QUICKDRAW_H
#include "missing/QDOffscreen.h"
#include "missing/Quickdraw.h"
#endif
#endif

// mac-qd-utils
#include "mac_qd/get_portRect.hh"

// nyancatlib
#include "nyancat/graphics.hh"

// NyanochromeCat
#include "Bitmap.hh"
#include "FrameDeltas.hh"
#include "Geometry.hh"


#define CONFIG_PORTBITS  ! TARGET_API_MAC_CARBON

#define CONFIG_FRAME_DELTAS  TARGET_CPU_68K


using mac::qd::get_portRect;

using nyancat::bitmap;
using nyancat::n_frames;


void bitmap::set_pixel( unsigned x, unsigned y, const Pattern& color )
{
	x *= its_magnifier;
	y *= its_magnifier;
	
	Rect r = { y, x, y + its_magnifier, x + its_magnifier };
	
	FillRect( &r, &color );
}

static bool using_frame_deltas;

static int current_frame;

const Pattern veryDarkGray = { 0x77, 0xFF, 0xDD, 0xFF, 0x77, 0xFF, 0xDD, 0xFF };

const short buffer_height = nyan_height * n_frames;
static Rect buffer_bounds = { 0, 0, buffer_height, nyan_width };
static BitMap buffer_bits = { 0, nyan_stride, buffer_bounds };

static GrafPtr offscreen_port;

static
void make_offscreen_port()
{
	short port_height = buffer_height;
	
	Ptr addr = NewPtr( frame_size * n_frames );
	
	if ( CONFIG_FRAME_DELTAS  &&  ! addr )
	{
		using_frame_deltas = true;
		
		port_height = nyan_height;
		
		const int n_cells = 3;
		
		addr = NewPtr( frame_size * n_cells );
	}
	
	if ( ! addr )
	{
		ExitToShell();
	}
	
	buffer_bits.baseAddr = addr;
	
#if CONFIG_PORTBITS
	
	offscreen_port = (GrafPtr) NewPtr( sizeof (GrafPort) );
	
	if ( ! offscreen_port )
	{
		ExitToShell();
	}
	
	OpenPort( offscreen_port );
	
	SetPortBits( &buffer_bits );
	
	PortSize( nyan_width, port_height );
	
	RectRgn( offscreen_port->visRgn, &offscreen_port->portRect );
	
#else
	
	QDErr err = NewGWorld( &offscreen_port, 1, &buffer_bounds, NULL, NULL, 0 );
	
	if ( err )
	{
		ExitToShell();
	}
	
	SetGWorld( offscreen_port, NULL );
	
#endif
}

void render_offscreen()
{
	make_offscreen_port();
	
	bitmap bits( zoom );
	
	FillRect( &buffer_bounds, &veryDarkGray );
	
	draw_frame( bits, 0 );
	
#if CONFIG_PORTBITS
	
	Ptr& baseAddr = offscreen_port->portBits.baseAddr;
	
#endif
	
	uint8_t* tmp = (uint8_t*) buffer_bits.baseAddr;
	
	uint8_t* alt = tmp + frame_size;
	uint8_t* dst = alt + frame_size;
	
	if ( using_frame_deltas )
	{
		// Copy frame 0 from cell 0 to cell 1.
		
		BlockMoveData( tmp, alt, frame_size );
	}
	
	for ( int i = 1;  i < n_frames;  ++i )
	{
	#if CONFIG_PORTBITS
		
		if ( using_frame_deltas )
		{
			baseAddr = (Ptr) dst;
			
			FillRect( &buffer_bounds, &veryDarkGray );
		}
		else
		{
			baseAddr += frame_size;
		}
		
	#else
		
		MovePortTo( 0, nyan_height * i );
		
	#endif
		
		draw_frame( bits, i );
		
		if ( using_frame_deltas )
		{
			uint8_t* src = dst;
			dst = alt;
			alt = src;
			
			save_frame_delta( i, dst, src );
		}
	}
	
	if ( using_frame_deltas )
	{
		uint8_t* src = tmp;
		
		dst = alt;
		
		save_frame_delta( 0, dst, src );
		
		SetPtrSize( buffer_bits.baseAddr, 2 * frame_size );
	}
	
#if CONFIG_PORTBITS
	
	ClosePort( offscreen_port );
	DisposePtr( (Ptr) offscreen_port );
	
#else
	
	MovePortTo( 0, 0 );
	
	CopyBits( GetPortBitMapForCopyBits( (CGrafPtr) offscreen_port ),
	          &buffer_bits,
	          &buffer_bounds,
	          &buffer_bounds,
	          srcCopy,
	          NULL );
	
	DisposeGWorld( offscreen_port );
	
#endif
	
	offscreen_port = NULL;
}

void prepare_next_frame()
{
	if ( ++current_frame == n_frames )
	{
		current_frame = 0;
	}
	
	if ( using_frame_deltas )
	{
		uint8_t* dst = (uint8_t*) buffer_bits.baseAddr;
		uint8_t* tmp = (uint8_t*) buffer_bits.baseAddr + frame_size;
		
		load_frame_delta( current_frame, dst, tmp );
	}
	else
	{
		buffer_bits.baseAddr += frame_size;
		
		if ( current_frame == 0 )
		{
			buffer_bits.baseAddr -= frame_size * n_frames;
		}
	}
}

void prepare_prev_frame()
{
	if ( using_frame_deltas )
	{
		uint8_t* dst = (uint8_t*) buffer_bits.baseAddr;
		uint8_t* tmp = (uint8_t*) buffer_bits.baseAddr + frame_size;
		
		load_frame_delta( current_frame, dst, tmp );
	}
	else
	{
		buffer_bits.baseAddr -= frame_size;
		
		if ( current_frame == 0 )
		{
			buffer_bits.baseAddr += frame_size * n_frames;
		}
	}
	
	if ( --current_frame < 0 )
	{
		current_frame = n_frames - 1;
	}
}

void blit( CGrafPtr port )
{
	const Rect& portRect = get_portRect( port );
	
	CopyBits( &buffer_bits,
	          GetPortBitMapForCopyBits( port ),
	          &portRect,
	          &portRect,
	          srcCopy,
	          NULL );
}
