/*
	Commands.cc
	-----------
	
	Joshua Juran
*/

#include "A-line/Commands.hh"

// Standard C++
#include <algorithm>
#include <functional>


namespace tool
{
	
	static const char* c_str( const plus::string& s )
	{
		return s.c_str();
	}
	
	Command& AugmentCommand( Command& command, const std::vector< plus::string >& more )
	{
		command.reserve( command.size() + more.size() );
		
		std::transform( more.begin(),
		                more.end(),
		                std::back_inserter( command ),
		                std::ptr_fun( c_str ));
		
		return command;
	}
	
}

