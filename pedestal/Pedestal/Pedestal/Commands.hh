/*
	Commands.hh
	-----------
	
	Copyright 2010, Joshua Juran
*/

#ifndef PEDESTAL_COMMANDS_HH
#define PEDESTAL_COMMANDS_HH

// Pedestal
#include "Pedestal/CommandCode.hh"


namespace Pedestal
{
	
	typedef bool (*CommandHandler)( CommandCode code );
	
	
	CommandHandler GetCommandHandler( CommandCode code );
	
	void SetCommandHandler( CommandCode code, CommandHandler handler );
	
}

#endif

