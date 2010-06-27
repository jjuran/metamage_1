/*	===============
 *	SimpleDevice.cc
 *	===============
 */

#include "Genie/IO/SimpleDevice.hh"

// plus
#include "plus/var_string.hh"

// Genie
#include "Genie/FS/ResolvePathname.hh"
#include "Genie/mmap/map_anonymous.hh"


namespace Genie
{
	
	FSTreePtr SimpleDeviceHandle::GetFile()
	{
		plus::var_string deviceName = "/dev/";
		
		deviceName += io.name;
		
		return ResolveAbsolutePath( deviceName );
	}
	
	ssize_t SimpleDeviceHandle::SysRead( char* data, std::size_t byteCount )
	{
		return io.reader( data, byteCount );
	}
	
	ssize_t SimpleDeviceHandle::SysWrite( const char* data, std::size_t byteCount )
	{
		return io.writer( data, byteCount );
	}
	
	memory_mapping::intrusive_ptr SimpleDeviceHandle::Map( size_t length, off_t offset )
	{
		return map_anonymous( length );
	}
	
}

