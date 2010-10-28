/*
	executable_file.cc
	------------------
*/

#include "Genie/code/executable_file.hh"

// Genie
#include "Genie/BinaryImage.hh"


namespace Genie
{
	
	execution_unit load_executable_file( const FSSpec& file )
	{
		return GetBinaryImage( file );
	}
	
}

