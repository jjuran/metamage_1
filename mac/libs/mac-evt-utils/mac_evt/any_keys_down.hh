/*
	any_keys_down.hh
	----------------
*/

#ifndef MACEVT_ANYKEYSDOWN_HH
#define MACEVT_ANYKEYSDOWN_HH

// Mac OS X
#ifdef __APPLE__
#include <Carbon/Carbon.h>
#endif

// Mac OS
#ifndef __EVENTS__
#include <Events.h>
#endif


namespace mac {
namespace evt {

inline
bool any_keys_down()
{
	KeyMap  keymap;
	UInt32* keys = (UInt32*) keymap;
	
	GetKeys( keymap );
	
	/*
		These will be endian-swapped on x86 and ARM,
		but we're only checking for zero, so it's fine.
	*/
	
	return keys[ 0 ]  ||  keys[ 1 ]  ||  keys[ 2 ]  ||  keys[ 3 ];
}

}
}

#endif
