/*
	Window.hh
	---------
*/

#ifndef TESTEDIT_WINDOW_HH
#define TESTEDIT_WINDOW_HH

// Mac OS X
#ifdef __APPLE__
#include <Carbon/Carbon.h>
#endif

// Mac OS
#ifndef __MACWINDOWS__
#include <MacWindows.h>
#endif

// plus
#include "plus/string_fwd.hh"


namespace TestEdit
{
	
	class ScrollFrame;
	
	typedef ScrollFrame View;
	
	WindowRef
	//
	NewWindow( ConstStr255Param title = "\p" "TestEdit" );
	
	void SetText( ScrollFrame& view, const plus::string& text );
	
}

#endif
