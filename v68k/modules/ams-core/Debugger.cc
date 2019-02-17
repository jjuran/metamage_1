/*
	Debugger.cc
	-----------
*/

#include "Debugger.hh"

// POSIX
#include <unistd.h>

// ams-common
#include "callouts.hh"


#define DEBUGGER_TEXT  "Debugger() was called!"


pascal void Debugger_patch()
{
	write( STDERR_FILENO, DEBUGGER_TEXT "\n", sizeof DEBUGGER_TEXT - 1 + 1 );
}

pascal void DebugStr_patch( const unsigned char* message )
{
	const size_t max_length = 255;
	
	char buffer[ max_length + 1 ];  // 256
	
	const size_t length = message[0];
	
	fast_memcpy( buffer, message + 1, length );
	
	buffer[ length ] = '\n';
	
	write( STDERR_FILENO, buffer, length + 1 );
}
