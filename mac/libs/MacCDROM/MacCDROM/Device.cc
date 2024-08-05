/*
	MacCDROM/Device.cc
	------------------
	
	Joshua Juran
*/

#ifdef __MACOS__
#include "MacCDROM/Device.hh"
#endif

// Mac OS
#ifndef __DEVICES__
#include <Devices.h>
#endif

// iota
#ifdef __GNUC__
#include "iota/dummy.hh"
#endif

// Nitrogen
#include "Mac/Toolbox/Utilities/ThrowOSStatus.hh"


namespace MacCDROM
{
	
#if CALL_NOT_IN_CARBON
	
	short OpenCDROMDriver()
	{
		/*
			We definitely don't want to close the driver.
			The question is, do we really need to open it at all,
			or could we just return the known reference number?
			
			Unless we're sure the driver is already open,
			it's better to play it safe.
		*/
		
		short refnum;
		
		OSErr err = OpenDriver( "\p" ".AppleCD", &refnum );
		
		Mac::ThrowOSStatus( err );
		
		return refnum;
	}
	
#endif  // CALL_NOT_IN_CARBON
	
}
