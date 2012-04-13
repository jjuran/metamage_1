/*
	node_ptr.hh
	-----------
*/

#ifndef VFS_NODEPTR_HH
#define VFS_NODEPTR_HH

// Debug
#include "debug/boost_assert.hh"

// boost
#include <boost/intrusive_ptr.hpp>


namespace vfs
{
	
	class node;
	
	typedef boost::intrusive_ptr< node const > node_ptr;
	
}

#endif

