/*
	Offscreen.hh
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


void render_offscreen();

void prepare_next_frame();
void prepare_prev_frame();

void blit( CGrafPtr port );
void draw( CGrafPtr port );

#endif
