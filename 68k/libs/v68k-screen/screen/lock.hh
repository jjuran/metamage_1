/*
	lock.hh
	-------
*/

#ifndef SCREEN_LOCK_HH
#define SCREEN_LOCK_HH


namespace v68k   {
namespace screen {

extern short lock_level;

extern bool ignoring_screen_locks;

inline
void ignore_screen_locks()
{
	ignoring_screen_locks = true;
}

inline
bool is_unlocked()
{
	return lock_level >= 0  ||  ignoring_screen_locks;
}

}  // namespace screen
}  // namespace v68k


#endif
