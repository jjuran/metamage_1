/*
	windows.hh
	----------
*/

#ifndef MACUI_WINDOWS_HH
#define MACUI_WINDOWS_HH

// Mac OS X
#ifdef __APPLE__
#include <Carbon/Carbon.h>
#endif

// Mac OS
#ifndef __MACWINDOWS__
#include <MacWindows.h>
#endif


namespace mac {
namespace ui  {
	
	Point get_window_position( WindowRef window );
	
	Point get_window_size( WindowRef window );
	
	bool invalidate_if_compositing( WindowRef window );
	
	void invalidate_window( WindowRef window );
	
#if ! __LP64__
	
	inline
	void set_window_title( WindowRef window, const Byte* name )
	{
		SetWTitle( window, name );
	}
	
#endif
	
	OSStatus set_window_title( WindowRef window, const HFSUniStr255& name );
	
	OSStatus set_window_title_UTF8( WindowRef window, const char* p, int n );
	
}
}

#endif
