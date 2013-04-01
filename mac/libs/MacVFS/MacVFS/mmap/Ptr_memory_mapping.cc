/*
	Ptr_memory_mapping.cc
	---------------------
*/

#include "MacVFS/mmap/Ptr_memory_mapping.hh"


namespace vfs
{
	
	Ptr_memory_mapping::Ptr_memory_mapping( const nucleus::shared< Mac::Ptr >&  p,
	                                        size_t                              length,
	                                        int                                 flags,
	                                        off_t                               offset )
	:
		memory_mapping( p.get() + offset, length, flags ),
		its_ptr( p )
	{
	}
	
}

