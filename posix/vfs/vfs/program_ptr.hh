/*
	program_ptr.hh
	--------------
*/

#ifndef VFS_PROGRAMPTR_HH
#define VFS_PROGRAMPTR_HH

// Debug
#include "debug/boost_assert.hh"

// boost
#include <boost/intrusive_ptr.hpp>


namespace vfs
{
	
	class program;
	
	typedef boost::intrusive_ptr< program > program_ptr;
	
}

#endif

