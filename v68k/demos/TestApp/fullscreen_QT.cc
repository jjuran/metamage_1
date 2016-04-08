/*
	fullscreen_QT.cc
	----------------
*/

#include "fullscreen_QT.hh"

// Mac OS X
#ifdef __APPLE__
#include <QuickTime/QuickTime.h>
#endif

// Mac OS
#ifndef __MOVIES__
#include <Movies.h>
#endif


static Ptr fullscreen_context;

void begin_fullscreen()
{
	if ( fullscreen_context == NULL )
	{
		WindowRef window;
		
		OSErr err = BeginFullScreen( &fullscreen_context,
		                             NULL,  // gd
		                             NULL,  // width
		                             NULL,  // height
		                             &window,
		                             NULL,  // color
		                             0 );
		
		SetPortWindowPort( window );
	}
}

void end_fullscreen()
{
	if ( fullscreen_context != NULL )
	{
		EndFullScreen( fullscreen_context, 0 );
		
		fullscreen_context = NULL;
	}
}
