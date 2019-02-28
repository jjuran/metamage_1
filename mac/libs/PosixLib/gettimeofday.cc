/*
	gettimeofday.cc
	---------------
*/

// Mac OS
#ifndef __DATETIMEUTILS__
#include <DateTimeUtils.h>
#endif

// POSIX
#include <sys/time.h>

// math
#include "math/integer.hh"

// mac-types
#include "mac_types/epoch.hh"

// mac-sys-utils
#include "mac_sys/gmt_delta.hh"
#include "mac_sys/microseconds.hh"


namespace PosixLib
{
	
	using mac::sys::microseconds;
	
	
	static inline UInt32 GetGlobalDateTime()
	{
		UInt32 result;
		
		::GetDateTime( &result );
		
		result -= mac::sys::gmt_delta();
		
		return result;
	}
	
	static inline time_t UnixTime()
	{
		return GetGlobalDateTime() - mac::types::epoch_delta();
	}
	
	static
	UInt64 time_offset( UInt64 microseconds )
	{
		using math::integer::long_multiply;
		
		UInt64 unix_microseconds = long_multiply( UnixTime(), 1000000 );
		
		return unix_microseconds - microseconds;
	}
	
	class MicrosecondUnixTimeClock
	{
		private:
			UInt64 offset;
		
		public:
			MicrosecondUnixTimeClock() : offset( time_offset( microseconds() ) )
			{
			}
			
			UInt64 Now()
			{
				const UInt64 now = microseconds();
				
				const UInt64 new_offset = time_offset( now );
				
				const int delta = 2 * 1000000;  // two seconds
				
				if ( new_offset - offset >= delta )
				{
					offset = new_offset;
				}
				
				return now + offset;
			}
	};
	
	
	static MicrosecondUnixTimeClock gClock;
	
}

using PosixLib::gClock;

int gettimeofday( struct timeval* tv, struct timezone* tz )
{
	if ( tv != NULL )
	{
		const UInt64 now = gClock.Now();
		
		tv->tv_sec  = now / 1000000;
		tv->tv_usec = now % 1000000;
	}
	
	return 0;
}
