/*
	Commands.cc
	-----------
	
	Copyright 2010, Joshua Juran
*/

#include "Pedestal/Commands.hh"

// Standard C++
#include <map>


namespace Pedestal
{
	
	typedef std::map< unsigned long, const void* > CommandHandlerMap;
	
	static CommandHandlerMap gCommandHandlers;
	
	
	CommandHandler GetCommandHandler( CommandCode code )
	{
		typedef CommandHandlerMap::const_iterator Iter;
		
		Iter it = gCommandHandlers.find( code );
		
		const void* result = it != gCommandHandlers.end() ? it->second : 0;
		
		return (CommandHandler) result;
	}
	
	void SetCommandHandler( CommandCode code, CommandHandler handler )
	{
		if ( handler != NULL )
		{
			gCommandHandlers[ code ] = (const void*) handler;
		}
		else
		{
			gCommandHandlers.erase( code );
		}
	}
	
}

