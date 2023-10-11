/*
	screen.cc
	---------
*/

#include "callout/screen.hh"

// v68k-screen
#include "screen/lock.hh"
#include "screen/update.hh"


#ifdef __MWERKS__
#pragma exceptions off
#endif


namespace v68k    {
namespace callout {

using v68k::screen::lock_level;


enum
{
	rts = 0x4E75,
};

int32_t lock_screen_callout( v68k::processor_state& s )
{
	--lock_level;
	
	return rts;
}

int32_t unlock_screen_callout( v68k::processor_state& s )
{
	if ( ++lock_level == 0 )
	{
		v68k::screen::update();
	}
	
	return rts;
}

}  // namespace callout
}  // namespace v68k
