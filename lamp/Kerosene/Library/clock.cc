/*
	clock.cc
	--------
*/

// Mac OS
#ifndef __TIMER__
#include <Timer.h>
#endif

// Standard C
#include "time.h"


#pragma exceptions off


clock_t clock( void )
{
	clock_t result;
	
	::Microseconds( (UnsignedWide*) &result );
	
	return result;
}

