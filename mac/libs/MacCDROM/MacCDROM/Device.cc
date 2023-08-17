/*
	MacCDROM/Device.cc
	------------------
	
	Joshua Juran
*/

#ifdef __MACOS__
#include "MacCDROM/Device.hh"
#endif

// iota
#ifdef __GNUC__
#include "iota/dummy.hh"
#endif

// MacCDROM
#include "MacCDROM/AppleCD.hh"


namespace MacCDROM
{
	
#if CALL_NOT_IN_CARBON
	
	Mac::DriverRefNum OpenCDROMDriver()
	{
		// We definitely don't want to close the driver.
		// The question is, do we really need to open it at all, or could we
		// just return the known reference number?
		// Unless we're sure the driver is already open, better to be safe
		// than stupid.
		return N::OpenDriver( "\p" ".AppleCD" ).release();
	}
	
#endif  // CALL_NOT_IN_CARBON
	
}
