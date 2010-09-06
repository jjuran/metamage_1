/*
	Commands.cc
	-----------
	
	Joshua Juran
*/

#include "A-line/Commands.hh"

// Standard C++
#include <algorithm>
#include <functional>

// Standard C
#include <stdlib.h>


namespace tool
{
	
	static const char* c_str( const plus::string& s )
	{
		return s.c_str();
	}
	
	Command& AugmentCommand( Command& command, const std::vector< plus::string >& more )
	{
		std::transform( more.begin(),
		                more.end(),
		                std::back_inserter( command ),
		                std::ptr_fun( c_str ));
		
		return command;
	}
	
	
	const char* CommandGenerator::UnixCompilerName() const
	{
		if ( const char* cc = getenv( "CC" ) )
		{
			return cc;
		}
		
		return gnu ? "cc" : "mwcc";
	}
	
	const char* CommandGenerator::LinkerName() const
	{
		if ( const char* ld = getenv( "LD" ) )
		{
			return ld;
		}
		
		return gnu ? "c++" : "ld";
	}
	
}
