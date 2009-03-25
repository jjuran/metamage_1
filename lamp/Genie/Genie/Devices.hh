/*	==========
 *	Devices.hh
 *	==========
 */

#ifndef GENIE_DEVICES_HH
#define GENIE_DEVICES_HH

// Standard C++
#include <string>

// Boost
#include <boost/shared_ptr.hpp>


namespace Genie
{
	
	class IOHandle;
	
	boost::shared_ptr< IOHandle > GetSimpleDeviceHandle( const std::string& path );
	
}

#endif

