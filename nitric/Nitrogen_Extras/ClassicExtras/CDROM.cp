/*	========
 *	CDROM.cp
 *	========
 */

#ifndef __MACH__
#ifndef CLASSICEXTRAS_CDROM_H
#include "ClassicExtras/CDROM.h"
#endif
#endif

// Nitrogen Core
#include "Nitrogen/Assert.h"

// Nitrogen Extras / ClassicExtras
#ifndef CLASSICEXTRAS_APPLECD_H
#include "ClassicExtras/AppleCD.h"
#endif


namespace NitrogenExtras
{
	
#if CALL_NOT_IN_CARBON
	
	N::DriverRefNum OpenCDROMDriver()
	{
		// We definitely don't want to close the driver.
		// The question is, do we really need to open it at all, or could we
		// just return the known reference number?
		// Unless we're sure the driver is already open, better to be safe
		// than stupid.
		return N::OpenDriver( ".AppleCD" ).Release();
	}
	
#endif  // CALL_NOT_IN_CARBON
	
}

