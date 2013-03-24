/*
	memory_mapping.cc
	-----------------
*/

#include "vfs/memory_mapping.hh"


namespace vfs
{
	
	memory_mapping::~memory_mapping()
	{
	}
	
	void memory_mapping::msync( void* addr, size_t len, int flags ) const
	{
	}
	
}

