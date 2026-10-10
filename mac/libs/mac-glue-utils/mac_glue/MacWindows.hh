/*
	MacWindows.hh
	-------------
*/

#ifndef MACGLUE_MACWINDOWS_HH
#define MACGLUE_MACWINDOWS_HH

#ifdef __APPLE__
#include <Carbon/Carbon.h>
#endif

#ifndef __MC68K__
#ifndef __MACWINDOWS__
#include <MacWindows.h>
#endif
#endif


#ifndef __MACWINDOWS__

typedef struct GrafPort* WindowRef;

#endif


namespace mac  {
namespace glue {
	
#ifdef __MC68K__
	
	enum
	{
		_SizeWindow = 0xA91D,
	};
	
	typedef unsigned char Boolean;
	
	/*
		Why split GrowWindow()'s result into two values
		which are then pushed as separate SizeWindow()
		parameters, when it can simply remain as is?
	*/
	
	inline
	asm
	pascal
	void SizeWindow( WindowRef window, long vh_data, Boolean update )
	{
		_SizeWindow
	}
	
#else  // #ifdef __MC68K__
	
	inline
	void SizeWindow( WindowRef window, long vh_data, Boolean update )
	{
		SizeWindow( window, vh_data, vh_data >> 16, update );
	}
	
#endif  // #else  // #ifdef __MC68K__
}
}

#endif
