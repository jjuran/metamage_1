/*
	window.hh
	---------
*/

#ifndef TICTACTOE_WINDOW_HH
#define TICTACTOE_WINDOW_HH

// Mac OS X
#ifdef __APPLE__
#include <ApplicationServices/ApplicationServices.h>
#endif

// Mac OS
#ifndef CGCONTEXT_H_
#ifndef __CGCONTEXT__
#include <CGContext.h>
#endif
#endif
#ifndef __QUICKDRAW__
#include <Quickdraw.h>
#endif

struct HFSUniStr255;

/*
	Compositing mode is introduced in Mac OS X 10.2,
	but doesn't work without major problems until 10.3.
*/

#ifdef MAC_OS_X_VERSION_10_3
#define CONFIG_COMPOSITING  1
#else
#define CONFIG_COMPOSITING  0
#endif

void draw_dock_tile();

inline
void propagate_to_dock_tile()
{
#ifdef __APPLE__
	
	draw_dock_tile();
	
#endif
}

void draw_window( CGContextRef context );
void draw_window( const Rect& portRect );

RgnHandle click_board( Point where );

void erase_token_in_region( RgnHandle rgn );

void calibrate_mouseRgns();

void window_size_changed( const Rect& portRect );

RgnHandle reset();

RgnHandle mouse_moved( Point where );

void reload( const unsigned char* data, unsigned short size );

void set_window_title( const Byte*         name );
void set_window_title( const HFSUniStr255& name );

inline
void set_window_untitled()
{
	return set_window_title( "\p" "Tic-tac-toe" );
}

#endif
