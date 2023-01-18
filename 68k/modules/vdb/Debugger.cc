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


#pragma exceptions off


#define DEBUGGER_TEXT  "User Break"


enum
{
	trace_on = 0xFFFFFFF4
};

static void Debugger_message()
{
	write( STDERR_FILENO, DEBUGGER_TEXT "\n", sizeof DEBUGGER_TEXT - 1 + 1 );
}

static void DebugStr_message( const unsigned char* message : __A0 )
{
	const size_t max_length = 255;
	
	char buffer[ max_length + 1 ];  // 256
	
	const size_t length = message[0];
	
	memcpy( buffer, message + 1, length );
	
	buffer[ length ] = '\n';
	
	write( STDERR_FILENO, buffer, length + 1 );
}

asm
pascal void Debugger_patch()
{
	LINK     A6,#0
	JSR      Debugger_message
	UNLK     A6
	
	JMP      trace_on
}

asm
pascal void DebugStr_patch( const unsigned char* message )
{
	MOVEA.L  4(SP),A0
	MOVE.L   (SP)+,(SP)
	
	LINK     A6,#0
	JSR      DebugStr_message
	UNLK     A6
	
	JMP      trace_on
}
