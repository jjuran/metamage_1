/*
	executable_file.hh
	------------------
*/

#ifndef GENIE_CODE_EXECUTABLEFILE_HH
#define GENIE_CODE_EXECUTABLEFILE_HH

// Genie
#include "Genie/code/execution_unit.hh"

// #include <Files.h>
struct FSSpec;


namespace Genie
{
	
	execution_unit load_executable_file( const FSSpec& file );
	
}

#endif

