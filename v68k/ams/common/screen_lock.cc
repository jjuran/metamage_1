/*
	screen_lock.cc
	--------------
*/

#include "screen_lock.hh"


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
