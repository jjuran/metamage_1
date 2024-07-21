/*
	board.hh
	--------
*/

#ifndef CHESS_BOARD_HH
#define CHESS_BOARD_HH

// Mac OS X
#ifdef __APPLE__
#include <ApplicationServices/ApplicationServices.h>
#endif

// Mac OS
#ifndef __QUICKDRAW__
#include <Quickdraw.h>
#endif


void click_board( Point where );

void reset();

#endif
