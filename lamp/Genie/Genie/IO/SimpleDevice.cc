/*	===============
 *	SimpleDevice.cc
 *	===============
 */

#include "Genie/IO/SimpleDevice.hh"

// Genie
#include "Genie/FileSystem/StatFile.hh"


namespace Genie
{
	
	int SimpleDeviceHandle::SysRead( char* data, std::size_t byteCount )
	{
		return io.reader( data, byteCount );
	}
	
	int SimpleDeviceHandle::SysWrite( const char* data, std::size_t byteCount )
	{
		return io.writer( data, byteCount );
	}
	
}

