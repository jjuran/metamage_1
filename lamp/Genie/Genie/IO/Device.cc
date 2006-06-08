/*	=========
 *	Device.cc
 *	=========
 */

#include "Genie/IO/Device.hh"

// Genie
#include "Genie/FileSystem/StatFile.hh"


namespace Genie
{
	
	void DeviceHandle::Stat( struct ::stat* sb ) const
	{
		StatGeneric( sb );
	}
	
}

