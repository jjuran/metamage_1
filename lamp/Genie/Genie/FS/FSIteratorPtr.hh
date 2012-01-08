/*
	FSIteratorPtr.hh
	----------------
*/

#ifndef GENIE_FS_FSITERATORPTR_HH
#define GENIE_FS_FSITERATORPTR_HH

// Debug
#include "debug/boost_assert.hh"

// boost
#include <boost/intrusive_ptr.hpp>


namespace Genie
{
	
	class FSIterator;
	
	typedef boost::intrusive_ptr< FSIterator > FSIteratorPtr;
	
}

#endif

