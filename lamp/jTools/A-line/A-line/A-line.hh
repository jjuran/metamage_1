/*	=========
 *	A-line.hh
 *	=========
 */

#if PRAGMA_ONCE
#pragma once
#endif

// Standard C++
#include <string>
#include <vector>

// POSeven
#include "POSeven/types/wait_t.hh"

// A-line
#include "A-line/Task.hh"


namespace tool
{
	
	struct OptionsRecord
	{
		bool all;
		bool verbose;
		bool catalog;
		
		OptionsRecord() : all    ( false ),
		                  verbose( false ),
		                  catalog( false )
		{
		}
	};
	
	OptionsRecord& Options();
	
	void ExecuteCommand( const TaskPtr&                     task,
	                     const std::string&                 caption,
	                     const std::vector< const char* >&  command,
	                     const char*                        diagnostics_file_path = NULL );
	
	void check_results( poseven::wait_t wait_status, const char* diagnostics_path );
	
}

