/*
	Debugger.cc
	-----------
*/

#include "Debugger.hh"

// POSIX
#include <unistd.h>

// ams-common
#include "callouts.hh"

// vdb
#include "trace.hh"


#pragma exceptions off


#define STR_LEN(s)  "" s, (sizeof s - 1)

#define DEBUGGER_TEXT  "User Break"
#define SYSERROR_TEXT  "System Error"


enum
{
	trace_on = 0xFFFFFFF4
};

static
void SysError_message( short error : __D0 )
{
	write( STDERR_FILENO, STR_LEN( SYSERROR_TEXT "\n" ) );
}

static
void Debugger_message()
{
	write( STDERR_FILENO, STR_LEN( DEBUGGER_TEXT "\n" ) );
}

static
void DebugStr_message( const unsigned char* message : __A0 )
{
	const size_t max_length = 255;
	
	char buffer[ max_length + 1 ];  // 256
	
	const size_t length = message[0];
	
	char* p = (char*) fast_mempcpy( buffer, message + 1, length );
	
	*p = '\n';
	
	write( STDERR_FILENO, buffer, length + 1 );
}

asm
void SysError_patch( short error : __D0 )
{
	LINK     A6,#0
	MOVEM.L  D0-D2/A0-A1,-(SP)
	
	JSR      SysError_message
	
	MOVEM.L  (SP)+,D0-D2/A0-A1
	UNLK     A6
	
	JMP      trace_on
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
