/*
	api.hh
	------
*/

#ifndef MWDEBUG_API_HH
#define MWDEBUG_API_HH


namespace mwdebug
{
	
	const unsigned char* MetroNubVersion();
	
	bool IsMetroNubInstalled();
	
	bool IsDebuggerRunning();
	
	bool AmIBeingDebugged();
	
}

#endif
