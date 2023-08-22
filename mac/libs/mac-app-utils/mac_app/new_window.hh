/*
	new_window.hh
	-------------
*/

#ifndef MACAPP_DOCUMENTS_HH
#define MACAPP_DOCUMENTS_HH

// Mac OS X
#ifdef __APPLE__
#include <Carbon/Carbon.h>
#endif

// Mac OS
#ifndef __MACWINDOWS__
#include <MacWindows.h>
#endif


#if ! __LP64__

namespace mac {
namespace app {
	
	extern const ProcPtr gNewWindow;
	
	inline
	pascal
#if TARGET_CPU_68K  &&  ! TARGET_RT_MAC_CFM
	asm
#endif
	WindowRef new_window( void*             storage,
	                      const Rect*       bounds,
	                      ConstStr255Param  title,
	                      Boolean           visible,
	                      short             procID,
	                      WindowRef         predecessor,
	                      Boolean           closable,
	                      long              refcon )
	{
	#if TARGET_CPU_68K  &&  ! TARGET_RT_MAC_CFM
		
		MOVEA.L   gNewWindow,A0
		JSR       (A0)
		
	#else
		
		return NewCWindow( storage, bounds, title,
		                   visible, procID, predecessor, closable, refcon );
		
	#endif
	}
	
	inline
	WindowRef new_window( const Rect&       bounds,
	                      ConstStr255Param  title,
	                      Boolean           visible,
	                      short             procID,
	                      WindowRef         predecessor,
	                      Boolean           closable,
	                      long              refcon = 0 )
	{
		return new_window( NULL, &bounds, title, visible,
		                   procID, predecessor, closable, refcon );
	}
	
}
}

#endif  // #if ! __LP64__

#endif
