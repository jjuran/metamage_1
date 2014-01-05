/*
	map_anonymous.cc
	----------------
*/

#include "MacVFS/mmap/map_anonymous.hh"

// Standard C
#include <string.h>

// Nitrogen
#include "Nitrogen/MacMemory.hh"

// MacVFS
#include "MacVFS/mmap/Handle_memory_mapping.hh"


namespace vfs
{
	
	namespace n = nucleus;
	namespace N = Nitrogen;
	
	
	memory_mapping_ptr map_anonymous( std::size_t length, int prot, int flags )
	{
		::Handle h = NULL;
		
		// dlmalloc defaults to 64K requests.  Don't use temp mem for those.
		
		if ( length <= 64 * 1024 )
		{
			h = ::NewHandleClear( length );
		}
		
		n::owned< Mac::Handle > handle;
		
		if ( h != NULL )
		{
			handle = n::owned< Mac::Handle >::seize( h );
		}
		else
		{
			// Allocation above either failed or wasn't attempted.
			
			handle = N::TempNewHandle( length );
			
			memset( *handle.get(), '\0', length );
		}
		
		return new Handle_memory_mapping( handle, length, flags );
	}
	
}

