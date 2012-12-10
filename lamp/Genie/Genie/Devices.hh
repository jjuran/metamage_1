/*	==========
 *	Devices.hh
 *	==========
 */

#ifndef GENIE_DEVICES_HH
#define GENIE_DEVICES_HH

// plus
#include "plus/string_fwd.hh"

// Genie
#include "Genie/IO/IOPtr.hh"


namespace Genie
{
	
	IOPtr GetSimpleDeviceHandle( const plus::string& path );
	
}

#endif

