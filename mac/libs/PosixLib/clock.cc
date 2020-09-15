/*
	clock.cc
	--------
*/

// POSIX
#include <time.h>

// mac-sys-utils
#include "mac_sys/microseconds.hh"


clock_t clock( void )
{
	return mac::sys::microseconds();
}
