/*
	SysError.cc
	-----------
*/

#include "SysError.hh"

// POSIX
#include <unistd.h>


#define STR_LEN( s )  "" s, (sizeof s - 1)


void SysError_patch( short error : __D0 )
{
	write( STDERR_FILENO, STR_LEN( "Sorry, a system error occurred.\n" ) );
	
	_exit( error );
}

static
asm void syserr_handler()
{
	MOVEQ.L  #0,D0
	MOVE.B   7(SP),D0
	LSR.W    #2,D0
	SUBQ.W   #1,D0
	JMP      SysError_patch
}

asm int set_syserr_handler()
{
	JSR      0xFFFFFFFA ;  // enter_supervisor_mode()
	
	BMI.S    bail ;  // D0 is -1 if branch taken
	
	LEA      syserr_handler,A0
	
	MOVE.L   A0,0x0008  // Bus Error
	MOVE.L   A0,0x000C  // Address Error
	MOVE.L   A0,0x0010  // Illegal Instruction
	MOVE.L   A0,0x0014  // Division By Zero
	MOVE.L   A0,0x0018  // CHK
	MOVE.L   A0,0x001C  // TRAPV
	MOVE.L   A0,0x0020  // Privilege Violation
	MOVE.L   A0,0x0024  // Trace
//	MOVE.L   A0,0x0028  // Line 1010
	MOVE.L   A0,0x002C  // Line 1111
	
	MOVE     D0,SR
	MOVEQ    #0,D0
	
bail:
	RTS
}
