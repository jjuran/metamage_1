/*	===============
 *	SimpleDevice.cc
 *	===============
 */

#include "Genie/IO/SimpleDevice.hh"

// Standard C++
#include <algorithm>

// plus
#include "plus/var_string.hh"

// poseven
#include "poseven/types/errno_t.hh"

// vfs
#include "vfs/node.hh"
#include "vfs/functions/resolve_pathname.hh"

// MacVFS
#include "MacVFS/mmap/map_anonymous.hh"

// Genie
#include "Genie/SystemConsole.hh"


namespace Genie
{
	
	namespace p7 = poseven;
	
	
	// General
	
	static ssize_t ReadNull( char*, std::size_t )
	{
		return 0;
	}
	
	static ssize_t WriteVoid( const char*, std::size_t byteCount )
	{
		return byteCount;
	}
	
	// Zero
	
	static ssize_t ReadZero( char* data, std::size_t byteCount )
	{
		std::fill( data,
		           data + byteCount,
		           '\0' );
		
		return byteCount;
	}
	
	// Console
	
	static ssize_t WriteConsole( const char* data, std::size_t byteCount )
	{
		return WriteToSystemConsole( data, byteCount );
	}
	
	static DeviceIOSpec gDeviceIOSpecs[] =
	{
		{ "null",    ReadNull, WriteVoid    },
		{ "zero",    ReadZero, WriteVoid    },
		{ "console", ReadNull, WriteConsole },
		{ NULL,      NULL,     NULL         }
	};
	
	static inline bool operator==( const DeviceIOSpec& device, const plus::string& s )
	{
		return device.name == s;
	}
	
	static const DeviceIOSpec* FindDevice( const plus::string& name )
	{
		const DeviceIOSpec* begin = gDeviceIOSpecs;
		const DeviceIOSpec* end   = begin + sizeof gDeviceIOSpecs / sizeof gDeviceIOSpecs[0];
		
		const DeviceIOSpec* it = std::find( begin, end, name );
		
		return it != end ? it : NULL;
	}
	
	
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
	
	
	vfs::filehandle_ptr GetSimpleDeviceHandle( const vfs::node& file )
	{
		if ( const DeviceIOSpec* device = FindDevice( file.name() ) )
		{
			return new SimpleDeviceHandle( *device );
		}
		
		throw p7::errno_t( ENOENT );
	}
	
}

