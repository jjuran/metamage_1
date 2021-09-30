/*
	Offscreen.hh
	------------
*/

// Mac OS X
#ifdef __APPLE__
#include <ApplicationServices/ApplicationServices.h>
#endif

// Mac OS
#ifndef __QUICKDRAW__
#include <Quickdraw.h>
#endif


extern int current_frame;

GrafPtr render_offscreen();

void prepare_next_frame();
void prepare_prev_frame();
