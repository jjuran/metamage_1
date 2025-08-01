/*
	Debugger.hh
	-----------
*/

#ifndef DEBUGGER_HH
#define DEBUGGER_HH


extern void* old_SysError;

void SysError_patch( short error : __D0 );

pascal void Debugger_patch();
pascal void DebugStr_patch( const unsigned char* message );

#endif
