/*
	CommandCode.hh
	--------------
*/

#ifndef PEDESTAL_COMMANDCODE_HH
#define PEDESTAL_COMMANDCODE_HH


namespace Pedestal
{
	
	enum CommandCode
	{
		kCmdNone = 0,
		
		kCmdAbout = 'abou',
		kCmdNew   = 'new ',
		
		kCommandCode_Max = 0xffffffff
	};
	
}

#endif

