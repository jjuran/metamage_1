/*	==========
 *	Devices.hh
 *	==========
 */

#ifndef GENIE_DEVICES_HH
#define GENIE_DEVICES_HH

// Debug
#include "debug/boost_assert.hh"

// Genie
#include "Genie/IO/IOPtr.hh"


namespace plus
{
	
	class string;
	
}

namespace Genie
{
	
	IOPtr GetSimpleDeviceHandle( const plus::string& path );
	
}

#endif

