/*	==========
 *	TimeOff.cc
 *	==========
 */

#include "TimeOff.hh"

// Nitrogen
#include "Nitrogen/OSUtils.h"


namespace TimeOff
{
	
	namespace N = Nitrogen;
	
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
	
	long GetGMTDelta()
	{
		long rawGMTDelta = N::ReadLocation().u.gmtDelta;
		
		long gmtDelta = MacLocalTimeDelta( rawGMTDelta );
		
		return gmtDelta;
	}
	
	UInt32 GlobalDateTime()
	{
		return N::GetDateTime() - MacToUnixTimeDifference( GetGMTDelta() );
	}
	
}

