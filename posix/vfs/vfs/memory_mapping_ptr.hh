/*
	memory_mapping_ptr.hh
	---------------------
*/

#ifndef VFS_MEMORYMAPPINGPTR_HH
#define VFS_MEMORYMAPPINGPTR_HH

// Debug
#include "debug/boost_assert.hh"

// boost
#include <boost/intrusive_ptr.hpp>


namespace vfs
{
	
	class memory_mapping;
	
	typedef boost::intrusive_ptr< memory_mapping > memory_mapping_ptr;
	
}

#endif

