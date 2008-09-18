/*	==========
 *	TimeOff.hh
 *	==========
 */

#ifndef TIMEOFF_HH
#define TIMEOFF_HH


namespace TimeOff
{
	
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
	
	inline UInt32 MacToUnixTimeDifference( long gmtDelta )
	{
		return MacUnixEpochOffset() + gmtDelta;
	}
	
	long GetGMTDelta();
	
	UInt32 GlobalDateTime();
	
}

#endif

