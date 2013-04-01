/*
	Handle_memory_mapping.hh
	------------------------
*/

#ifndef MACVFS_MMAP_HANDLEMEMORYMAPPING_HH
#define MACVFS_MMAP_HANDLEMEMORYMAPPING_HH

// nucleus
#ifndef NUCLEUS_OWNED_HH
#include "nucleus/owned.hh"
#endif

// Nitrogen
#ifndef MAC_MEMORY_TYPES_HANDLE_HH
#include "Mac/Memory/Types/Handle.hh"
#endif

// vfs
#include "vfs/memory_mapping.hh"


namespace vfs
{
	
	class Handle_memory_mapping : public memory_mapping
	{
		private:
			nucleus::owned< Mac::Handle > its_handle;
		
		public:
			Handle_memory_mapping( nucleus::owned< Mac::Handle > h, Size size, int flags );
	};
	
}

#endif

