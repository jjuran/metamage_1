/*
	gmt_delta.hh
	------------
*/

#ifndef MACSYS_GMTDELTA_HH
#define MACSYS_GMTDELTA_HH


namespace mac {
namespace sys {
	
	// Gets the raw machineLocation.u.gmtDelta
	long gmtDelta_field();
	
	// Returns whether DLS is in effect
	inline
	bool gmtDelta_dls( long gmtDelta )
	{
		return gmtDelta & 0x80000000;
	}
	
	// Gets the refined GMT delta from the raw field.
	// Call it with loc.u.gmtDelta (without masking the high byte) after calling
	// ReadLocation( &loc ).
	inline
	long gmtDelta_gmt_delta( long gmtDelta )
	{
		// Mask off DLS byte, and sign extend if negative
		
		return (gmtDelta & 0x00FFFFFF) | (gmtDelta & 0x00800000) * 0xFF << 1;
	}
	
	// Returns the delta in seconds between global time and Mac system time.
	// Subtract the result from a Mac system date to get a GMT date (Mac epoch).
	// Add it to a GMT date to get a Mac system date.
	inline
	long gmt_delta()
	{
		return gmtDelta_gmt_delta( gmtDelta_field() );
	}
	
}
}

#endif
