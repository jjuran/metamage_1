/*
	screen_lock.cc
	--------------
*/

#include "screen_lock.hh"

// Mac OS
#ifndef __RESOURCES__
#include <Resources.h>
#endif

// POSIX
#include <unistd.h>

// Standard C
#include <stdlib.h>
#include <string.h>

// macos
#include "options.hh"


static inline
asm void lock_screen()
{
	JSR  0xFFFFFFEC
}

static inline
asm void unlock_screen()
{
	JSR  0xFFFFFFEA
}

screen_lock::screen_lock()
{
	if ( synchronized_quickdraw )
	{
		lock_screen();
	}
}

screen_lock::~screen_lock()
{
	if ( synchronized_quickdraw )
	{
		unlock_screen();
	}
}
