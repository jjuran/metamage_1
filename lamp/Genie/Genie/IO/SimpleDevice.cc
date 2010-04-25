/*	===============
 *	SimpleDevice.cc
 *	===============
 */

#include "Genie/IO/SimpleDevice.hh"

// Nitrogen
#include "Nitrogen/MacMemory.hh"

// Genie
#include "Genie/FS/ResolvePathname.hh"
#include "Genie/mmap/Handle_memory_mapping.hh"


namespace Genie
{
	
	namespace N = Nitrogen;
	
	
	FSTreePtr SimpleDeviceHandle::GetFile()
	{
		std::string deviceName = "/dev/";
		
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
	
	memory_mapping::shared_ptr SimpleDeviceHandle::Map( size_t length, off_t offset )
	{
		return seize_ptr( new Handle_memory_mapping( N::NewHandleClear( length ) ) );
	}
	
}

