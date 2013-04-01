/*
	Handle_memory_mapping.cc
	------------------------
*/

#include "MacVFS/mmap/Handle_memory_mapping.hh"

// Nitrogen
#ifndef NITROGEN_MACMEMORY_HH
#include "Nitrogen/MacMemory.hh"
#endif


namespace vfs
{
	
	namespace N = Nitrogen;
	
	
	Handle_memory_mapping::Handle_memory_mapping( nucleus::owned< Mac::Handle >  h,
	                                              Size                           size,
	                                              int                            flags )
	:
		memory_mapping( *h.get(), size, flags ),
		its_handle( h )
	{
		N::HLock( its_handle );
	}
	
}

