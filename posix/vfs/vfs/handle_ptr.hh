/*
	handle_ptr.hh
	-------------
*/

#ifndef VFS_HANDLEPTR_HH
#define VFS_HANDLEPTR_HH

// Debug
#include "debug/boost_assert.hh"

// boost
#include <boost/intrusive_ptr.hpp>


namespace vfs
{
	
	class handle;
	
	typedef boost::intrusive_ptr< handle > handle_ptr;
	
}

#endif

