/*	=========
 *	A-line.hh
 *	=========
 *	
 *	Implemented by A-line.cc
 */

#pragma once

// Standard C++
#include <string>
#include <vector>


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
	
	void ExecuteCommand( const std::vector< const char* >& command, const char* diagnosticsFilename = NULL );
	
}

