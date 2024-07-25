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
#include "mac_qd/copy_bits.hh"
#include "mac_qd/get_portRect.hh"

// nyancatlib
#include "nyancat/graphics.hh"

// NyanochromeCat
#include "Bitmap.hh"
#include "FrameDeltas.hh"
#include "Geometry.hh"


#pragma exceptions off


#define CONFIG_PORTBITS  ! TARGET_API_MAC_CARBON

#define CONFIG_FRAME_DELTAS  TARGET_CPU_68K


using mac::qd::get_portRect;

using nyancat::bitmap;
using nyancat::n_frames;


bool using_frame_deltas;

Ptr temp_blit_buffer;

static inline
void my_memcpy( void* dst, const void* src, long n )
{
	BlockMoveData( src, dst, n );
}

void bitmap::set_pixel( Coord x, Coord y, Color color )
{
	x *= zoom;
	y *= zoom;
	
	Rect r = { y, x, y + zoom, x + zoom };
	
	FillRect( &r, &color );
}

void bitmap::fill_rect( Coord x, Coord y, Delta dx, Delta dy, Color color )
{
	x *= zoom;
	y *= zoom;
	
	dx *= zoom;
	dy *= zoom;
	
	Rect r = { y, x, y + dy, x + dx };
	
	FillRect( &r, &color );
}

static int current_frame;

const Pattern veryDarkGray = { 0x77, 0xFF, 0xDD, 0xFF, 0x77, 0xFF, 0xDD, 0xFF };

const short buffer_height = nyan_height * n_frames;

static BitMap buffer_bits =
{
	NULL,
	nyan_stride,
	{ 0, 0, buffer_height, nyan_width },
};

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
	
	const Rect& buffer_bounds = buffer_bits.bounds;
	
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
	
	bitmap bits;
	
	FillRect( &buffer_bits.bounds, &veryDarkGray );
	
	draw_frame( bits, 0 );
	
#if CONFIG_PORTBITS
	
	Ptr& baseAddr = offscreen_port->portBits.baseAddr;
	
#endif
	
	Byte* tmp = (Byte*) buffer_bits.baseAddr;
	
	Byte* dst = tmp + frame_size;
	Byte* alt = dst + frame_size;
	
	if ( using_frame_deltas )
	{
		// Copy frame 0 from cell 0 to cell 2.
		
		BlockMoveData( tmp, alt, frame_size );
	}
	
	for ( int i = 1;  i < n_frames;  ++i )
	{
	#if CONFIG_PORTBITS
		
		if ( using_frame_deltas )
		{
			baseAddr = (Ptr) dst;
			
			FillRect( &buffer_bits.bounds, &veryDarkGray );
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
			Byte* src = dst;
			dst = alt;
			alt = src;
			
			save_frame_delta( i, dst, src );
		}
	}
	
	if ( using_frame_deltas )
	{
		SetPtrSize( buffer_bits.baseAddr, 2 * frame_size );
		
		Byte* src = tmp;
		dst = alt;
		
		save_frame_delta( 0, dst, src );
	}
	
#if CONFIG_PORTBITS
	
	ClosePort( offscreen_port );
	DisposePtr( (Ptr) offscreen_port );
	
#else
	
	MovePortTo( 0, 0 );
	
	mac::qd::copy_bits( offscreen_port,
	                    buffer_bits,
	                    buffer_bits.bounds );
	
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
		Byte* dst = (Byte*) buffer_bits.baseAddr;
		Byte* tmp = (Byte*) buffer_bits.baseAddr + frame_size;
		
		load_frame_delta( current_frame, dst, tmp );
	}
	else
	{
		Ptr prev_baseAddr = buffer_bits.baseAddr;
		
		buffer_bits.baseAddr += frame_size;
		
		if ( current_frame == 0 )
		{
			buffer_bits.baseAddr -= frame_size * n_frames;
		}
		
		/*
			If the temporary XOR blitting buffer is allocated,
			populate it with a frame delta of the current frame
			and the next frame.  This won't occur in Carbon (or if
			we're already using frame deltas to save memory).
		*/
		
		if ( CALL_NOT_IN_CARBON  &&  temp_blit_buffer )
		{
			const Byte* next = (Byte*) buffer_bits.baseAddr;
			
			my_memcpy( temp_blit_buffer, prev_baseAddr, frame_size );
			
			memxor_aligned_32b( (Byte*) temp_blit_buffer, next, frame_size );
		}
	}
}

void prepare_prev_frame()
{
	if ( using_frame_deltas )
	{
		Byte* dst = (Byte*) buffer_bits.baseAddr;
		Byte* tmp = (Byte*) buffer_bits.baseAddr + frame_size;
		
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
	ptrdiff_t addr_diff = frame_size;
	
	if ( ! using_frame_deltas )
	{
		if ( ! temp_blit_buffer )
		{
			draw( port );
			
			return;
		}
		
		/*
			We're not using frame deltas generally, but we need
			to use them temporarily if the buffer is allocated.
		*/
		
		addr_diff = temp_blit_buffer - buffer_bits.baseAddr;
	}
	
	buffer_bits.baseAddr += addr_diff;
	
	mac::qd::copy_bits( buffer_bits,
	                    port,
	                    get_portRect( port ),
	                    srcXor );
	
	buffer_bits.baseAddr -= addr_diff;
}

void draw( CGrafPtr port )
{
	const Rect& portRect = get_portRect( port );
	
	mac::qd::copy_bits( buffer_bits,
	                    port,
	                    portRect );
}
