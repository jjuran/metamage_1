/*
	draw_CG.hh
	----------
*/

#ifndef TICTACTOE_DRAWCG_HH
#define TICTACTOE_DRAWCG_HH

// Mac OS X
#ifdef __APPLE__
// Including just <CoreGraphics/CoreGraphics.h> fails in Mac OS X 10.4.
#include <ApplicationServices/ApplicationServices.h>
#endif

// Mac OS
#ifndef CGCONTEXT_H_
#ifndef __CGCONTEXT__
#include <CGContext.h>
#endif
#endif


void draw_token( CGContextRef context, int token, int x, int y );

void draw_board( CGContextRef context, signed char board[ 9 ] );

#endif
