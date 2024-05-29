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
		kHICommandPaste     = 'past',
		kHICommandSelectAll = 'sall',
		
		kCmdNone = 0,
		
		kCmdAbout = 'abou',
		kCmdOpen  = 'open',
		kCmdNew   = 'new ',
		
		kCmdClose = 'clos',
		kCmdQuit  = 'quit',
		
		kCmdUndo  = 'undo',
		kCmdCut   = 'cut ',
		kCmdCopy  = 'copy',
		kCmdPaste = kHICommandPaste,
		kCmdClear = 'clea',
		
		kCmdSelectAll = kHICommandSelectAll,
		
		kCommandCode_Max = 0xffffffff
	};
	
}

#endif
