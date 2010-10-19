/*
	shared_exec_handle.hh
	---------------------
*/

#ifndef GENIE_CODE_SHAREDEXECHANDLE_HH
#define GENIE_CODE_SHAREDEXECHANDLE_HH

// Debug
#include "debug/boost_assert.hh"

// Boost
#include <boost/intrusive_ptr.hpp>

// Genie
#include "Genie/code/exec_handle.hh"


namespace Genie
{
	
	typedef boost::intrusive_ptr< exec_handle >  shared_exec_handle;
	
}

#endif

