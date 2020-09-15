/*
	os.hh
	-----
*/

#ifndef VLIB_OS_HH
#define VLIB_OS_HH

// Standard C
#include <signal.h>


namespace vlib
{
	
	extern unsigned char yield_countdown;
	
	inline
	void periodic_yield()
	{
	#ifdef __RELIX__
		
		if ( --yield_countdown == 0 )
		{
			kill( 1, 0 );
		}
		
	#endif
	}
	
}

#endif
