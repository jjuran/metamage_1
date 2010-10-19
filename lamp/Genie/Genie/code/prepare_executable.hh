/*
	prepare_executable.hh
	---------------------
*/

#ifndef GENIE_CODE_PREPAREEXECUTABLE_HH
#define GENIE_CODE_PREPAREEXECUTABLE_HH

// Genie
#include "Genie/code/execution_unit.hh"
#include "Genie/code/shared_exec_handle.hh"


namespace Genie
{
	
	shared_exec_handle prepare_executable( const execution_unit& unit );
	
}

#endif

