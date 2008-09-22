// ============
// Execution.hh
// ============

#ifndef EXECUTION_HH
#define EXECUTION_HH

// Standard C++
#include <string>

// POSeven
#include "POSeven/types/wait_t.hh"


namespace tool
{
	
	poseven::wait_t ExecuteCmdLine( const std::string& cmd );
	
}

#endif

