// ============
// Execution.hh
// ============

#ifndef EXECUTION_HH
#define EXECUTION_HH

// Standard C++
#include <string>

// poseven
#include "poseven/types/wait_t.hh"


namespace tool
{
	
	poseven::wait_t ExecuteCmdLine( const std::string& cmd );
	
	poseven::wait_t ExecuteCmdLine( const char* cmd );
	
}

#endif

