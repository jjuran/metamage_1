/*
	dock_tile.hh
	------------
*/

#ifndef DOCKTILE_HH
#define DOCKTILE_HH

// Mac OS X
#ifdef __APPLE__
#include <ApplicationServices/ApplicationServices.h>
#endif

// Mac OS
#ifndef __QUICKDRAW__
#include <Quickdraw.h>
#endif


void draw_dock_tile( signed char board[ 9 ] );

void propagate_to_dock_tile( short unitLength, Point margin );

#endif
