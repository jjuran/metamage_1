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
#ifndef __QUICKDRAW__
#include <Quickdraw.h>
#endif


RgnHandle alloc_mouseRgns();

void propagate_to_dock_tile();

void draw_window( const Rect& portRect );

RgnHandle click( Point where, bool sound_enabled );

RgnHandle undo();

void calibrate_mouseRgns();

void window_size_changed( const Rect& portRect );

RgnHandle reset();

RgnHandle mouse_moved( Point where );

#endif
