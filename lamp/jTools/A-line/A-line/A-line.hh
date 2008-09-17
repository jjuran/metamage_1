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
	
}

