/*
	filehandle_ptr.hh
	-----------------
*/

#ifndef VFS_FILEHANDLEPTR_HH
#define VFS_FILEHANDLEPTR_HH

// Debug
#include "debug/boost_assert.hh"

// boost
#include <boost/intrusive_ptr.hpp>


namespace vfs
{
	
	class filehandle;
	
	typedef boost::intrusive_ptr< filehandle > filehandle_ptr;
	
}

#endif

