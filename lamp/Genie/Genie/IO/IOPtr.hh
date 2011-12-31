/*
	IOPtr.hh
	--------
*/

#ifndef GENIE_IO_IOPTR_HH
#define GENIE_IO_IOPTR_HH

// boost
#include <boost/shared_ptr.hpp>


namespace Genie
{
	
	class IOHandle;
	
	typedef boost::shared_ptr< IOHandle > IOPtr;
	
}

#endif

