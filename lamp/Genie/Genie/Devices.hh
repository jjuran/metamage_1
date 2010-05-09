/*	==========
 *	Devices.hh
 *	==========
 */

#ifndef GENIE_DEVICES_HH
#define GENIE_DEVICES_HH

// Debug
#include "debug/boost_assert.hh"

// Boost
#include <boost/shared_ptr.hpp>


namespace plus
{
	
	class string;
	
}

namespace Genie
{
	
	class IOHandle;
	
	boost::shared_ptr< IOHandle > GetSimpleDeviceHandle( const plus::string& path );
	
}

#endif

