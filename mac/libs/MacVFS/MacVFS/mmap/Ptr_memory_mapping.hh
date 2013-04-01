/*
	Ptr_memory_mapping.hh
	---------------------
*/

#ifndef MACVFS_MMAP_HANDLEMEMORYMAPPING_HH
#define MACVFS_MMAP_HANDLEMEMORYMAPPING_HH

// POSIX
#include <sys/types.h>

// nucleus
#ifndef NUCLEUS_SHARED_HH
#include "nucleus/shared.hh"
#endif

// Nitrogen
#ifndef MAC_MEMORY_TYPES_HANDLE_HH
#include "Mac/Memory/Types/Ptr.hh"
#endif

// vfs
#include "vfs/memory_mapping.hh"


namespace vfs
{
	
	class Ptr_memory_mapping : public memory_mapping
	{
		private:
			nucleus::shared< Mac::Ptr > its_ptr;
		
		public:
			Ptr_memory_mapping( const nucleus::shared< Mac::Ptr >&  p,
			                    size_t                              length,
			                    int                                 flags,
			                    off_t                               offset );
	};
	
}

#endif

