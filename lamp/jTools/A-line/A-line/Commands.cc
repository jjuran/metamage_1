/*
	Commands.cc
	-----------
	
	Joshua Juran
*/

#include "A-line/Commands.hh"

// Standard C
#include <stdlib.h>


namespace tool
{
	
	Command& AugmentCommand( Command& command, const StringVector& more )
	{
		for ( size_t i = 0;  i < more.size();  ++i )
		{
			command.push_back( more[ i ].c_str() );
		}
		
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
