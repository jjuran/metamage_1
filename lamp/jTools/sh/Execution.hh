// ============
// Execution.hh
// ============

#ifndef EXECUTION_HH
#define EXECUTION_HH

// poseven
#include "poseven/types/wait_t.hh"


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

