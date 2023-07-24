/*
	clock.hh
	--------
*/

#ifndef MACSYS_CLOCK_HH
#define MACSYS_CLOCK_HH

// Mac OS X
#ifdef __APPLE__
#include <CoreServices/CoreServices.h>
#endif

#if TARGET_API_MAC_CARBON
#ifndef __OSUTILS__
#include <OSUtils.h>
#endif
#endif

// mac-sys-utils
#include "mac_sys/microseconds.hh"


namespace mac {
namespace sys {
	
	typedef unsigned long       tick_clock_t;
	typedef unsigned long long  microsecond_clock_t;
	
	inline
	bool get_ticks( tick_clock_t* t )
	{
	#if TARGET_API_MAC_CARBON
		
		*t = TickCount();  // Ticks
		
	#else
		
		*t = *(tick_clock_t*) 0x016A;  // Ticks
		
	#endif
		
		return true;
	}
	
	namespace tick_clock
	{
		
		using ::mac::sys::get_ticks;
		
		enum
		{
			clocks_per_kilosecond = 60150,  // 60.15 Hz
		};
		
		typedef tick_clock_t clock_t;
		
		inline
		void get( clock_t* t )
		{
			get_ticks( t );
		}
		
		inline
		tick_clock_t ticks_from( const clock_t t )
		{
			return t;
		}
		
	}
	
	namespace microsecond_clock
	{
		
		enum
		{
			clocks_per_kilosecond = 1000000 * 1000,
		};
		
		typedef microsecond_clock_t clock_t;
		
		inline
		void get( clock_t* t )
		{
			microseconds( t );
		}
		
		inline
		tick_clock_t ticks_from( const clock_t t )
		{
			if ( t < 71403831296000ull )
			{
				return t / 16625;  // 1/1000000 -> 1/60.15
			}
			
			return tick_clock_t( -1 );
		}
		
	}
	
}
}

#endif
