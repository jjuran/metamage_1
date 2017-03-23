/*
	Controls.cc
	-----------
*/

#include "Controls.hh"

// Mac OS
#ifndef __CONTROLS__
#include <Controls.h>
#endif
#ifndef __MACWINDOWS__
#include <MacWindows.h>
#endif


pascal void DisposeControl_patch( ControlRecord** control )
{
}

pascal void KillControls_patch( GrafPort* window )
{
	WindowPeek w = (WindowPeek) window;
	
	while ( ControlRef control = (ControlRef) w->controlList )
	{
		DisposeControl( control );
	}
}
