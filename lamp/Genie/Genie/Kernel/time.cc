/*	=======
 *	time.cc
 *	=======
 */

// Standard C++
#include <algorithm>

// POSIX
#include "sys/time.h"

// Nucleus
#include "Nucleus/NAssert.h"

// Nitrogen
#include "Nitrogen/OSUtils.h"

// Genie
#include "Genie/SystemCallRegistry.hh"
#include "Genie/SystemCalls.hh"


namespace Genie
{
	
	DECLARE_MODULE_INIT( Kernel_time )
	DEFINE_MODULE_INIT(  Kernel_time )
	
	namespace N = Nitrogen;
	
	inline unsigned long MacUnixEpochOffset()
	{
		// Returns the number of seconds between Mac and Unix epochs (global time).
		// Mac time - Unix time = offset
		// Mac time - offset = Unix time
		// Unix time + offset = Mac time
		
		enum { macYear = 1904, unixYear = 1970 };
		
		const unsigned years = unixYear - macYear;  // 66
		const unsigned quads = years / 4;  // 16
		const unsigned extra = years % 4;  // 2
		const unsigned daysPerQuad = 4 * 365 + 1;  // One Feb 29 per four years
		const unsigned extraDays = extra * 365 + 1;  // First year is a leap year
		const unsigned daysOffset = daysPerQuad * quads + extraDays;
		const unsigned secondsPerDay = 60 * 60 * 24;
		const unsigned long kMacUnixEpochOffset = daysOffset * secondsPerDay;
		
		return kMacUnixEpochOffset;
	}
	
	static long MacLocalTimeDelta( long gmtDelta )
	{
		// Returns the delta in seconds between global time and Mac system time.
		// Call it with loc.u.gmtDelta (without masking the high byte) after calling
		// ReadLocation( &loc ).
		// Subtract the result from a Mac system date to get a GMT date (Mac epoch).
		// Add it to a GMT date to get a Mac system date.
		
		// 7th bit of high byte indicates Daylight Saving Time is in effect
		bool dls = gmtDelta & 0x80000000;
		
		// Mask off DLS byte
		gmtDelta &= 0x00FFFFFF;
		
		// gmtDelta is the number of seconds ahead of GMT we are.
		// For EST, it's -5 * 3600 = -18000.  For EDT, it's -4 * 3600 = -14400.
		
		// If delta is negative we need to sign-extend it
		bool signExtend = gmtDelta & 0x00800000;
		
		// Sign-extend if required
		gmtDelta |= signExtend ? 0xFF000000 : 0x00000000;
		
		return gmtDelta;
	}
	
	
	static int gettimeofday( struct timeval* tv, struct timezone* tz )
	{
		SystemCallFrame frame( "gettimeofday" );
		
		long gmtDelta = N::ReadLocation().u.gmtDelta;
		
		gmtDelta = MacLocalTimeDelta( gmtDelta );
		
		const unsigned long timeDiff = MacUnixEpochOffset() + gmtDelta;
		
		if ( tv != NULL )
		{
			tv->tv_sec  = N::GetDateTime() - timeDiff;
			tv->tv_usec = 0;
		}
		
		if ( tz != NULL )
		{
			tz->tz_minuteswest = -gmtDelta / 60;
			tz->tz_dsttime     = 0;
		}
		
		return 0;
	}
	
	REGISTER_SYSTEM_CALL( gettimeofday );
	
}

