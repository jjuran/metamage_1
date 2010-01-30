/*
	Commands.hh
	-----------
	
	Copyright 2010, Joshua Juran
*/

#ifndef PEDESTAL_COMMANDS_HH
#define PEDESTAL_COMMANDS_HH


namespace Pedestal
{
	
	enum CommandCode
	{
		kCmdAbout = 'abou',
		kCmdNew   = 'new ',
		
		kCommandCode_Max = 0xffffffff
	};
	
	
	typedef bool (*CommandHandler)( CommandCode code );
	
	
	CommandHandler GetCommandHandler( CommandCode code );
	
	void SetCommandHandler( CommandCode code, CommandHandler handler );
	
}

#endif

