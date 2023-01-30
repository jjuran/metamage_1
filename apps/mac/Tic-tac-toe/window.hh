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


/*
	Compositing mode is introduced in Mac OS X 10.2,
	but doesn't work without major problems until 10.3.
*/

#ifdef MAC_OS_X_VERSION_10_3
#define CONFIG_COMPOSITING  1
#else
#define CONFIG_COMPOSITING  0
#endif

void propagate_to_dock_tile();

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
