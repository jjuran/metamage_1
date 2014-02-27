/*	===============
 *	SimpleDevice.cc
 *	===============
 */

#include "Genie/IO/SimpleDevice.hh"

// POSIX
#include <fcntl.h>

// Standard C++
#include <algorithm>

// plus
#include "plus/string.hh"

// poseven
#include "poseven/types/errno_t.hh"

// vfs
#include "vfs/node.hh"
#include "vfs/filehandle/methods/filehandle_method_set.hh"
#include "vfs/filehandle/methods/general_method_set.hh"
#include "vfs/filehandle/methods/stream_method_set.hh"
#include "vfs/mmap/functions/map_anonymous.hh"

// Genie
#include "Genie/IO/Stream.hh"
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
	
	typedef ssize_t (*Reader)( char      *, std::size_t );
	typedef ssize_t (*Writer)( char const*, std::size_t );
	
	struct DeviceIOSpec
	{
		const char* name;
		Reader reader;
		Writer writer;
	};
	
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
	
	static const DeviceIOSpec& FindDevice( const plus::string& name )
	{
		const DeviceIOSpec* begin = gDeviceIOSpecs;
		const DeviceIOSpec* end   = begin + sizeof gDeviceIOSpecs / sizeof gDeviceIOSpecs[0];
		
		const DeviceIOSpec* it = std::find( begin, end, name );
		
		if ( it == NULL )
		{
			throw p7::errno_t( ENOENT );
		}
		
		return *it;
	}
	
	
	static memory_mapping_ptr
	//
	devzero_mmap( vfs::filehandle* that, size_t length, int prot, int flags, off_t offset )
	{
		return vfs::map_anonymous( length, prot, flags );
	}
	
	static const vfs::general_method_set devzero_general_methods =
	{
		&devzero_mmap,
	};
	
	
	class SimpleDeviceHandle : public StreamHandle
	{
		private:
			const DeviceIOSpec& io;
		
		public:
			SimpleDeviceHandle( const vfs::node& file );
			
			ssize_t SysRead( char* data, std::size_t byteCount );
			
			ssize_t SysWrite( const char* data, std::size_t byteCount );
	};
	
	
	static ssize_t simpledevice_read( vfs::filehandle* that, char* buffer, size_t n )
	{
		return static_cast< SimpleDeviceHandle& >( *that ).SysRead( buffer, n );
	}
	
	static ssize_t simpledevice_write( vfs::filehandle* that, const char* buffer, size_t n )
	{
		return static_cast< SimpleDeviceHandle& >( *that ).SysWrite( buffer, n );
	}
	
	static const vfs::stream_method_set simpledevice_stream_methods =
	{
		NULL,
		&simpledevice_read,
		&simpledevice_write,
	};
	
	static const vfs::filehandle_method_set simpledevice_methods =
	{
		NULL,
		NULL,
		&simpledevice_stream_methods,
		&devzero_general_methods,
	};
	
	
	SimpleDeviceHandle::SimpleDeviceHandle( const vfs::node& file )
	:
		StreamHandle( &file, O_RDWR, &simpledevice_methods ),
		io( FindDevice( file.name() ) )
	{
	}
	
	ssize_t SimpleDeviceHandle::SysRead( char* data, std::size_t byteCount )
	{
		return io.reader( data, byteCount );
	}
	
	ssize_t SimpleDeviceHandle::SysWrite( const char* data, std::size_t byteCount )
	{
		return io.writer( data, byteCount );
	}
	
	
	vfs::filehandle_ptr GetSimpleDeviceHandle( const vfs::node& file )
	{
		return new SimpleDeviceHandle( file );
	}
	
}

