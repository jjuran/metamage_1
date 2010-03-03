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
	UnsignedWide result;
	
	::Microseconds( &result );
	
	return result.lo;
}

