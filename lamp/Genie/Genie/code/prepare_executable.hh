/*
	prepare_executable.hh
	---------------------
*/

#ifndef GENIE_CODE_PREPAREEXECUTABLE_HH
#define GENIE_CODE_PREPAREEXECUTABLE_HH

// vfs
#include "vfs/program_ptr.hh"

// vfs-relix
#include "vfs/program.hh"

// Genie
#include "Genie/code/execution_unit.hh"


namespace Genie
{
	
	vfs::program_ptr prepare_executable( const execution_unit& unit );
	
}

#endif

