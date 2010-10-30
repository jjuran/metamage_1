/*
	execution_unit.hh
	-----------------
*/

#ifndef GENIE_CODE_EXECUTIONUNIT_HH
#define GENIE_CODE_EXECUTIONUNIT_HH

// nucleus
#ifndef NUCLEUS_SHARED_HH
#include "nucleus/shared.hh"
#endif

// Nitrogen
#ifndef MAC_MEMORY_TYPES_HANDLE_HH
#include "Mac/Memory/Types/Handle.hh"
#endif


namespace Genie
{
	
	typedef nucleus::shared< Mac::Handle > execution_unit;
	
}

#endif

