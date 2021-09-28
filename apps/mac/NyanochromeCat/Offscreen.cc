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

// nyancatlib
#include "nyancat/graphics.hh"

// NyanochromeCat
#include "Bitmap.hh"
#include "Geometry.hh"


#define CONFIG_PORTBITS  ! TARGET_API_MAC_CARBON


using nyancat::bitmap;
using nyancat::n_frames;


void bitmap::set_pixel( unsigned x, unsigned y, const Pattern& color )
{
	x *= its_magnifier;
	y *= its_magnifier;
	
	Rect r = { y, x, y + its_magnifier, x + its_magnifier };
	
	FillRect( &r, &color );
}

const Pattern veryDarkGray = { 0x77, 0xFF, 0xDD, 0xFF, 0x77, 0xFF, 0xDD, 0xFF };

const short buffer_height = nyan_height * n_frames;
static Rect buffer_bounds = { 0, 0, buffer_height, nyan_width };
static BitMap buffer_bits = { 0, nyan_stride, buffer_bounds };

static GrafPtr offscreen_port;

static
void make_offscreen_port()
{
	Ptr addr = NewPtr( frame_size * n_frames );
	
	if ( ! addr )
	{
		ExitToShell();
	}
	
	buffer_bits.baseAddr = addr;
	
#if CONFIG_PORTBITS
	
	static GrafPort port;
	
	offscreen_port = &port;
	
	OpenPort( offscreen_port );
	
	RectRgn( offscreen_port->visRgn, &buffer_bounds );
	
	SetPortBits( &buffer_bits );
	
	PortSize( nyan_width, buffer_height );
	
#else
	
	QDErr err = NewGWorld( &offscreen_port, 1, &buffer_bounds, NULL, NULL, 0 );
	
	if ( err )
	{
		ExitToShell();
	}
	
	SetGWorld( offscreen_port, NULL );
	
#endif
}

GrafPtr render_offscreen()
{
	make_offscreen_port();
	
	bitmap bits( zoom );
	
	FillRect( &buffer_bounds, &veryDarkGray );
	
	draw_frame( bits, 0 );
	
	for ( int i = 1;  i < n_frames;  ++i )
	{
		MovePortTo( 0, nyan_height * i );
		
		draw_frame( bits, i );
	}
	
	MovePortTo( 0, 0 );
	
	return offscreen_port;
}
