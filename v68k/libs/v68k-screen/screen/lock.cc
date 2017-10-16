/*
	lock.cc
	-------
*/

#include "screen/lock.hh"


#pragma exceptions off


namespace v68k   {
namespace screen {

short lock_level;

static bool ignoring_screen_locks;


void ignore_screen_locks()
{
	ignoring_screen_locks = true;
}

bool is_unlocked()
{
	return lock_level >= 0  ||  ignoring_screen_locks;
}

}  // namespace screen
}  // namespace v68k
