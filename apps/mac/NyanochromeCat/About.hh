/*
	About.hh
	--------
*/

#ifndef ABOUT_HH
#define ABOUT_HH

// Mac OS X
#ifdef __APPLE__
#include <ApplicationServices/ApplicationServices.h>
#endif

// Mac OS
#ifndef __QUICKDRAW__
#include <Quickdraw.h>
#endif


enum
{
	kAboutWindowKind = 256,
};

void draw_About_box();
void show_About_box();

bool close_About_box( WindowRef window );

#endif
