/*
	lock.hh
	-------
*/

#ifndef SCREENLOCK_HH
#define SCREENLOCK_HH


namespace v68k   {
namespace screen {

extern short lock_level;

void ignore_screen_locks();

bool is_unlocked();

}  // namespace screen
}  // namespace v68k


#endif
