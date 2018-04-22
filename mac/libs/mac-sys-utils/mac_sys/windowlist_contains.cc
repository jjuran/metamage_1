/*
	windowlist_contains.hh
	----------------------
*/

#include "mac_sys/windowlist_contains.hh"

// Mac OS X
#ifdef __APPLE__
#include <Carbon/Carbon.h>
#endif

// Mac OS
#ifndef __MACWINDOWS__
#include <MacWindows.h>
#endif


namespace mac {
namespace sys {
	
#if ! __LP64__
	
	bool windowlist_contains( void* window )
	{
		WindowRef w = GetWindowList();
		
		while ( w != NULL )
		{
			if ( w == window )
			{
				return true;
			}
			
			w = GetNextWindow( w );
		}
		
		return false;
	}
	
#else
	
	void dummy()
	{
	}
	
#endif
	
}
}
