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
		kCmdOpen  = 'open',
		kCmdNew   = 'new ',
		
		kCmdClose = 'clos',
		kCmdQuit  = 'quit',
		
		kCmdUndo  = 'undo',
		kCmdCut   = 'cut ',
		kCmdCopy  = 'copy',
		kCmdPaste = 'pste',
		kCmdClear = 'clea',
		
		kCmdSelectAll = 'slct',
		
		kHICommandPaste     = 'past',  // kHICommandPaste,
		kHICommandSelectAll = 'sall',  // kHICommandSelectAll,
		
		kCommandCode_Max = 0xffffffff
	};
	
}

#endif
