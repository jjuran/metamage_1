/*	==========
 *	TimeOff.hh
 *	==========
 */

#ifndef TIMEOFF_HH
#define TIMEOFF_HH


namespace TimeOff
{
	
	// Gets the raw machineLocation.u.gmtDelta
	long GetGMTDeltaField();
	
	// Returns whether DLS is in effect
	inline bool GetDLSFromGMTDeltaField( long gmtDeltaField )
	{
		return gmtDeltaField & 0x80000000;
	}
	
	// Gets the refined GMT delta from the raw field.
	// Call it with loc.u.gmtDelta (without masking the high byte) after calling
	// ReadLocation( &loc ).
	inline long GetGMTDeltaFromField( long field )
	{
		// Mask off DLS byte, and sign extend if negative
		// (Look, mom, no branches!)
		return (field & 0x00FFFFFF) | (field & 0x00800000) * 0xFF << 1;
	}
	
	// Returns the delta in seconds between global time and Mac system time.
	// Subtract the result from a Mac system date to get a GMT date (Mac epoch).
	// Add it to a GMT date to get a Mac system date.
	inline long GetGMTDelta()
	{
		return GetGMTDeltaFromField( GetGMTDeltaField() );
	}
	
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
	
	inline unsigned long MacToUnixTimeDifference()
	{
		return MacUnixEpochOffset() + GetGMTDelta();
	}
	
}

#endif

