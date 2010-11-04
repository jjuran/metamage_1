// ============
// Execution.hh
// ============

#ifndef EXECUTION_HH
#define EXECUTION_HH

// poseven
#ifndef POSEVEN_TYPES_WAIT_T_HH
#include "poseven/types/wait_t.hh"
#endif


namespace plus
{
	
	class string;
	
}

namespace tool
{
	
	poseven::wait_t ExecuteCmdLine( const plus::string& cmd );
	
	poseven::wait_t ExecuteCmdLine( const char* cmd );
	
}

#endif

