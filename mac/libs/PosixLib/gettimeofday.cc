/*
	gettimeofday.cc
	---------------
*/

// Mac OS
#ifndef __DATETIMEUTILS__
#include <DateTimeUtils.h>
#endif
#ifndef __TIMER__
#include <Timer.h>
#endif

// POSIX
#include <sys/time.h>

// TimeOff
#include "TimeOff/TimeOff.hh"


namespace PosixLib
{
	
	static inline UInt32 GetGlobalDateTime()
	{
		UInt32 result;
		
		::GetDateTime( &result );
		
		result -= TimeOff::GetGMTDelta();
		
		return result;
	}
	
	static inline time_t UnixTime()
	{
		return GetGlobalDateTime() - TimeOff::MacUnixEpochOffset();
	}
	
	static inline UInt64 Microseconds()
	{
		UInt64 result;
		
		::Microseconds( (UnsignedWide*) &result );
		
		return result;
	}
	
	struct Clock
	{
		UInt64 diff;
		
		Clock() : diff( UnixTime() * 1000000ULL - Microseconds() )
		{
		}
		
		UInt64 Now() const  { return Microseconds() + diff; }
	};
	
	
	static Clock gClock;
	
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

