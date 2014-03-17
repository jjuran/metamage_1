/*
	Debugger.cc
	-----------
*/

#include "Debugger.hh"

// POSIX
#include <string.h>
#include <unistd.h>

// vdb
#include "trace.hh"


#define DEBUGGER_TEXT  "User Break"


inline asm void trace_on()
{
	JSR  0xFFFFFFF4
}

pascal void Debugger_patch()
{
	write( STDERR_FILENO, DEBUGGER_TEXT "\n", sizeof DEBUGGER_TEXT - 1 + 1 );
	
	trace_on();
}

pascal void DebugStr_patch( const unsigned char* message )
{
	const size_t max_length = 255;
	
	char buffer[ max_length + 1 ];  // 256
	
	const size_t length = message[0];
	
	memcpy( buffer, message + 1, length );
	
	buffer[ length ] = '\n';
	
	write( STDERR_FILENO, buffer, length + 1 );
	
	trace_on();
}

