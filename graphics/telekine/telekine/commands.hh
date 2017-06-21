/*
	commands.hh
	-----------
*/

#ifndef TELEKINE_COMMANDS_HH
#define TELEKINE_COMMANDS_HH

// telekine
#include "telekine/mb32.hh"


namespace telekine
{
	
	enum sender_command
	{
		Command_sender_none = 0
	};
	
	const sender_command Command_desc = sender_command( mb32( 'D', 'E', 'S', 'C' ) );
	const sender_command Command_rect = sender_command( mb32( 'R', 'E', 'C', 'T' ) );
	
	enum receiver_command
	{
		Command_receiver_none = 0
	};
	
	const receiver_command Command_step = receiver_command( mb32( 'S', 'T', 'E', 'P' ) );
	const receiver_command Command_play = receiver_command( mb32( 'P', 'L', 'A', 'Y' ) );
	
}

#endif
