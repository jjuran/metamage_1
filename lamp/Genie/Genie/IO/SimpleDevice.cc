/*	===============
 *	SimpleDevice.cc
 *	===============
 */

#include "Genie/IO/SimpleDevice.hh"

// plus
#include "plus/var_string.hh"

// vfs
#include "vfs/node.hh"
#include "vfs/functions/resolve_pathname.hh"

// MacVFS
#include "MacVFS/mmap/map_anonymous.hh"


namespace Genie
{
	
	FSTreePtr SimpleDeviceHandle::GetFile()
	{
		plus::var_string deviceName = "/dev/";
		
		deviceName += io.name;
		
		return vfs::resolve_absolute_path( deviceName );
	}
	
	ssize_t SimpleDeviceHandle::SysRead( char* data, std::size_t byteCount )
	{
		return io.reader( data, byteCount );
	}
	
	ssize_t SimpleDeviceHandle::SysWrite( const char* data, std::size_t byteCount )
	{
		return io.writer( data, byteCount );
	}
	
	memory_mapping_ptr
	//
	SimpleDeviceHandle::Map( size_t length, int prot, int flags, off_t offset )
	{
		return vfs::map_anonymous( length, prot, flags );
	}
	
}

