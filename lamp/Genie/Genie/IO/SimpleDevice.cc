/*	===============
 *	SimpleDevice.cc
 *	===============
 */

#include "Genie/IO/SimpleDevice.hh"

// POSIX
#include <fcntl.h>

// Standard C
#include <string.h>

// vfs
#include "vfs/filehandle.hh"
#include "vfs/memory_mapping.hh"
#include "vfs/node.hh"
#include "vfs/filehandle/primitives/get_file.hh"
#include "vfs/filehandle/methods/filehandle_method_set.hh"
#include "vfs/filehandle/methods/general_method_set.hh"
#include "vfs/filehandle/methods/stream_method_set.hh"
#include "vfs/mmap/functions/map_anonymous.hh"

// relix
#include "relix/fs/console.hh"


namespace Genie
{
	
	static vfs::memory_mapping_ptr
	//
	devzero_mmap( vfs::filehandle* that, size_t length, int prot, int flags, off_t offset )
	{
		return vfs::map_anonymous( length, prot, flags );
	}
	
	static const vfs::general_method_set devzero_general_methods =
	{
		&devzero_mmap,
	};
	
	
	static ssize_t simpledevice_read( vfs::filehandle* that, char* buffer, size_t n )
	{
		const char c = get_file( *that )->name()[ 0 ];
		
		if ( c == 'z' )
		{
			memset( buffer, '\0', n );
			return n;
		}
		
		return 0;
	}
	
	static ssize_t simpledevice_write( vfs::filehandle* that, const char* buffer, size_t n )
	{
		const char c = get_file( *that )->name()[ 0 ];
		
		if ( c == 'c' )
		{
			return relix::console::log( buffer, n );
		}
		
		return n;
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
	
	
	vfs::filehandle_ptr GetSimpleDeviceHandle( const vfs::node& file )
	{
		return new vfs::filehandle( &file, O_RDWR, &simpledevice_methods );
	}
	
}
