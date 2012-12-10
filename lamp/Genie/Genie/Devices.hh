/*	==========
 *	Devices.hh
 *	==========
 */

#ifndef GENIE_DEVICES_HH
#define GENIE_DEVICES_HH

// Debug
#include "debug/boost_assert.hh"

// plus
#include "plus/string_fwd.hh"

// Genie
#include "Genie/IO/IOPtr.hh"


namespace Genie
{
	
	IOPtr GetSimpleDeviceHandle( const plus::string& path );
	
}

#endif

