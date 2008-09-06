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

// CompileDriver
#include "CompileDriver/Platform.hh"

// A-line
#include "A-line/TargetInfo.hh"


namespace ALine
{
	
	namespace CD = CompileDriver;
	
	struct OptionsRecord
	{
		bool all;
		bool verbose;
		bool catalog;
		CD::Platform platform;
		
		OptionsRecord()
		:
			verbose( false ), 
			catalog( false )
		{}
	};
	
	OptionsRecord& Options();
	
	void ExecuteCommand( const std::vector< const char* >& command, const char* diagnosticsFilename = NULL );
	
}

