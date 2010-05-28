/*	===============
 *	SimpleDevice.cc
 *	===============
 */

#include "Genie/IO/SimpleDevice.hh"

// Standard C
#include <string.h>

// plus
#include "plus/var_string.hh"

// Nitrogen
#include "Nitrogen/MacMemory.hh"

// Genie
#include "Genie/FS/ResolvePathname.hh"
#include "Genie/mmap/Handle_memory_mapping.hh"


namespace Genie
{
	
	namespace n = nucleus;
	namespace N = Nitrogen;
	
	
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
		::Handle h = NULL;
		
		// dlmalloc defaults to 64K requests.  Don't use temp mem for those.
		
		if ( length < 128 * 1024 )
		{
			h = ::NewHandleClear( length );
		}
		
		n::owned< N::Handle > handle;
		
		if ( h != NULL )
		{
			handle = n::owned< N::Handle >::seize( h );
		}
		else
		{
			// Allocation above either failed or wasn't attempted.
			
			handle = N::TempNewHandle( length );
			
			memset( *handle.get(), '\0', length );
		}
		
		return seize_ptr( new Handle_memory_mapping( handle ) );
	}
	
}

