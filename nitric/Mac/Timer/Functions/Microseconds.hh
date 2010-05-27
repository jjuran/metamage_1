/*
	Mac/Timer/Functions/Microseconds.hh
	-----------------------------------
*/

#ifndef MAC_TIMER_FUNCTIONS_MICROSECONDS_HH
#define MAC_TIMER_FUNCTIONS_MICROSECONDS_HH

// Mac OS
#ifndef __TIMER__
#include <Timer.h>
#endif


namespace Mac
{
	
	inline UInt64 Microseconds()
	{
		::UnsignedWide result;
		
		::Microseconds( &result );
		
		return *reinterpret_cast< UInt64* >( &result );
	}
	
}

#endif

