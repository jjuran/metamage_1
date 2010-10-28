/*
	prepare_executable.cc
	------------------
*/

#include "Genie/code/prepare_executable.hh"

// Genie
#ifdef __POWERPC__
#include "Genie/code/fragment_handle.hh"
#else
#include "Genie/code/resource_handle.hh"
#endif


namespace Genie
{
	
	shared_exec_handle prepare_executable( const execution_unit& unit )
	{
		return new native_code_handle( unit );
	}
	
}

