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


static inline
asm void inline_lock_screen()
{
	JSR  0xFFFFFFEC
}

static inline
asm void inline_unlock_screen()
{
	JSR  0xFFFFFFEA
}

void lock_screen()
{
	inline_lock_screen();
}

void unlock_screen()
{
	inline_unlock_screen();
}
