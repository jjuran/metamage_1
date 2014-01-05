/*
	malloc_memory_mapping.hh
	------------------------
*/

#ifndef VFS_MMAP_TYPES_MALLOCMEMORYMAPPING_HH
#define VFS_MMAP_TYPES_MALLOCMEMORYMAPPING_HH

// POSIX
#include <sys/types.h>

// vfs
#include "vfs/memory_mapping.hh"


namespace vfs
{
	
	class malloc_memory_mapping : public memory_mapping
	{
		private:
			// non-copyable
			malloc_memory_mapping           ( const malloc_memory_mapping& );
			malloc_memory_mapping& operator=( const malloc_memory_mapping& );
		
		public:
			malloc_memory_mapping( void* addr, size_t size, int flags )
			:
				memory_mapping( addr, size, flags )
			{
			}
			
			~malloc_memory_mapping();
	};
	
}

#endif

