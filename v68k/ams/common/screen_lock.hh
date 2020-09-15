/*
	screen_lock.hh
	--------------
*/

#ifndef SCREENLOCK_HH
#define SCREENLOCK_HH

// ams-common
#include "callouts.hh"


class screen_lock
{
	private:
		// non-copyable
		screen_lock           ( const screen_lock& );
		screen_lock& operator=( const screen_lock& );
	
	public:
		screen_lock()
		{
			lock_screen();
		}
		
		~screen_lock()
		{
			unlock_screen();
		}
};

#endif
