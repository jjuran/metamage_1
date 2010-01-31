/*
	SetPort_GetWindow.hh
	--------------------
	
	Copyright 2009-2010, Joshua Juran
*/

#ifndef PEDESTAL_SETPORTGETWINDOW_HH
#define PEDESTAL_SETPORTGETWINDOW_HH

// Mac OS
#ifndef __MACWINDOWS__
#include <MacWindows.h>
#endif


namespace Pedestal
{
	
	class Window;
	
	Window* SetPort_GetWindow( WindowRef windowRef );
	
	inline Window* SetPort_FrontWindow()
	{
		return SetPort_GetWindow( ::FrontWindow() );
	}
	
}

#endif

