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

// vfs
#include "vfs/program_ptr.hh"

// Genie
#include "Genie/code/exec_handle.hh"


namespace Genie
{
	
	typedef vfs::program_ptr shared_exec_handle;
	
}

#endif

