/*
	Handle_memory_mapping.cc
	------------------------
*/

#include "MacVFS/mmap/Handle_memory_mapping.hh"

// Nitrogen
#ifndef MAC_MEMORY_TYPES_HANDLE_HH
#include "Mac/Memory/Types/Handle.hh"
#endif


namespace vfs
{
	
	Handle_memory_mapping::Handle_memory_mapping( nucleus::owned< Mac::Handle >  h,
	                                              Size                           size,
	                                              int                            flags )
	:
		memory_mapping( *h.get(), size, flags ),
		its_handle( h )
	{
		HLock( its_handle.get() );
	}
	
}
