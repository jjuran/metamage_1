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
		bool verbose;
		bool catalog;
		CD::Platform platform;
		std::vector< std::string > files;
		
		OptionsRecord()
		:
			verbose( false ), 
			catalog( false )
		{}
	};
	
	OptionsRecord& Options();
	
	void QueueCommand( const std::string& command );
	
}

