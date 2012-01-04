/*
	clock.c
	-------
*/

// Mac OS
#ifndef __TIMER__
#include <Timer.h>
#endif

// POSIX
#include <time.h>


clock_t clock( void )
{
	UInt64 result;
	
	Microseconds( (UnsignedWide*) &result );
	
	return result;
}

