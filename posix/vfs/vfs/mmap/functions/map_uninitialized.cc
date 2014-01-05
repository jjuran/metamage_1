/*
	map_uninitialized.cc
	--------------------
*/

#include "vfs/mmap/functions/map_uninitialized.hh"

// Standard C
#include <errno.h>
#include <stdlib.h>

// poseven
#include "poseven/types/errno_t.hh"

// vfs
#include "vfs/mmap/functions/map_temporary.hh"
#include "vfs/mmap/types/malloc_memory_mapping.hh"


#ifdef __MACOS__
#define NO_TEMPMEM  0
#else
#define NO_TEMPMEM  1
#endif


namespace vfs
{
	
	namespace p7 = poseven;
	
	
#if NO_TEMPMEM
	
	inline memory_mapping_ptr map_temporary( std::size_t length, int prot, int flags )
	{
		p7::throw_errno( ENOMEM );
		
		// Not reached
		return memory_mapping_ptr();
	}
	
#endif
	
	memory_mapping_ptr map_uninitialized( std::size_t length, int prot, int flags )
	{
		// dlmalloc defaults to 64K requests.  Don't use temp mem for those.
		
		if ( NO_TEMPMEM  ||  length <= 64 * 1024 )
		{
			if ( void* addr = malloc( length ) )
			{
				try
				{
					return new malloc_memory_mapping( addr, length, flags );
				}
				catch ( ... )
				{
					free( addr );
				}
			}
		}
		
		// Allocation above either failed or wasn't attempted.
		
		return map_temporary( length, prot, flags );
	}
	
}

