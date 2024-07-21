/*
	offscreen.hh
	------------
*/

#ifndef OFFSCREEN_HH
#define OFFSCREEN_HH

// Mac OS X
#ifdef __APPLE__
#include <ApplicationServices/ApplicationServices.h>
#endif

// Mac OS
#ifndef __QUICKDRAW__
#include <Quickdraw.h>
#endif


extern GrafPtr offscreen_port;

void set_offscreen_chess_board_size( short x );

void blit( GrafPtr port );

void animate_icon( GrafPtr port, BitMap& icon_bits, const Rect& rect, short mode );

#endif
