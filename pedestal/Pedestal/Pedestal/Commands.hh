/*
	Commands.hh
	-----------
	
	Copyright 2010, Joshua Juran
*/

#ifndef PEDESTAL_COMMANDS_HH
#define PEDESTAL_COMMANDS_HH

// Pedestal
#ifndef PEDESTAL_COMMANDCODE_HH
#include "Pedestal/CommandCode.hh"
#endif


namespace Pedestal
{
	
	typedef bool (*CommandHandler)( CommandCode code );
	
	
	CommandHandler GetCommandHandler( CommandCode code );
	
	void SetCommandHandler( CommandCode code, CommandHandler handler );
	
}

#endif

