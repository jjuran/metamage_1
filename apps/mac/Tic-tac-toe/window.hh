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

// mac-config
#include "mac_config/compositing-mode.hh"


extern WindowRef main_window;

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

#endif
