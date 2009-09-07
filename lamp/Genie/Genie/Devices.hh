/*	==========
 *	Devices.hh
 *	==========
 */

#ifndef GENIE_DEVICES_HH
#define GENIE_DEVICES_HH

// Standard C++
#include <string>

// Debug
#include "debug/boost_assert.hh"

// Boost
#include <boost/shared_ptr.hpp>


namespace Genie
{
	
	class IOHandle;
	
	boost::shared_ptr< IOHandle > GetSimpleDeviceHandle( const std::string& path );
	
}

#endif

