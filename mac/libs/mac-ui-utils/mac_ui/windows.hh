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
	void invalidate_thePort_window_rect( WindowRef window, const Rect& rect )
	{
		/*
			Portable code must pass a window that is already set
			as the current port.  However, a caller *may* choose
			to rely on this function's implementation, loosening
			the requirements:  Carbon callers need not set their
			window as the current port, while non-Carbon callers
			need not pass a window at all (though it must be set
			as the current port).
			
			That said, non-portable callers that are built either
			only as Carbon or only as non-Carbon can (and should)
			just use InvalRect() or InvalWindowRect() explicitly.
		*/
		
	#if CALL_NOT_IN_CARBON
		
		InvalRect( &rect );
		
	#else
		
		InvalWindowRect( window, &rect );
		
	#endif
	}
	
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
