/*	==========
 *	Devices.hh
 *	==========
 */

#ifndef GENIE_DEVICES_HH
#define GENIE_DEVICES_HH

// Boost
#include <boost/shared_ptr.hpp>


namespace Genie
{
	
	class SimpleDeviceHandle;
	
	boost::shared_ptr< SimpleDeviceHandle > GetSimpleDeviceHandle( const char* path );
	
}

#endif

