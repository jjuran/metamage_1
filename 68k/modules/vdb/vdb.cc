/*
	vdb.cc
	------
*/

// Mac OS
#ifndef __TRAPS__
#include <Traps.h>
#endif

// POSIX
#include <unistd.h>

// Standard C
#include <string.h>

// vdb
#include "Debugger.hh"
#include "trace.hh"


#define STRLEN( s )  (sizeof "" s - 1)

#define STR_LEN( s )  "" s, (sizeof s - 1)


void* os_trap_table     [] : 1 * 1024;
void* toolbox_trap_table[] : 3 * 1024;

#define TBTRAP( Routine )  (toolbox_trap_table[ _ ## Routine & 0x03FF ] = &Routine ## _patch)

static void install_Debugger()
{
	TBTRAP( SysError );  // A9C9
	
	TBTRAP( Debugger );  // A9FF
	TBTRAP( DebugStr );  // ABFF
}

static char hex[] =
{
	'0', '1', '2', '3',
	'4', '5', '6', '7',
	'8', '9', 'A', 'B',
	'C', 'D', 'E', 'F'
};

#define UNIMPLEMENTED_TRAP_PREFIX  "\n    *** Unimplemented A-trap: "
#define UNIMPLEMENTED_TRAP_MSG     UNIMPLEMENTED_TRAP_PREFIX       "A123 ***\n"

static
void report_unimplemented_trap( unsigned short trap_word : __D1 )
{
	char buffer[] = UNIMPLEMENTED_TRAP_MSG;
	
	char* p = buffer + STRLEN( UNIMPLEMENTED_TRAP_PREFIX );
	
	p[ 1 ] = hex[ trap_word >> 8 & 0xF ];
	p[ 2 ] = hex[ trap_word >> 4 & 0xF ];
	p[ 3 ] = hex[ trap_word      & 0xF ];
	
	write( STDERR_FILENO, buffer, STRLEN( UNIMPLEMENTED_TRAP_MSG ) );
}

static
asm void unimplemented_trap()
{
	/*
		Save the volatile registers onto the stack, print our report,
		restore the registers, and jump to the user break handler.
		
		OS and Toolbox traps are not documented as preserving the CCR,
		nor to this author's knowledge do they in any way read the CCR.
		So we don't need to save and restore it here; it's sufficient to
		ensure that we don't leak any information through it.
	*/
	
	MOVEM.L  D0-D2/A0-A1,-(SP)
	
	JSR      report_unimplemented_trap
	
	MOVEM.L  (SP)+,D0-D2/A0-A1
	ANDI     #0,CCR
	
	JMP      user_break
}

static
void reassign_unimplemented_traps()
{
	void* old_addr = toolbox_trap_table[ _Unimplemented & 0x03FF ];
	void* new_addr = &unimplemented_trap;
	
	short n;
	void** p;
	
	n = 256;
	p = os_trap_table;
	
	while ( --n >= 0 )
	{
		if ( *p++ == old_addr )
		{
			p[ -1 ] = new_addr;
		}
	}
	
	n = 1024;
	p = toolbox_trap_table;
	
	while ( --n >= 0 )
	{
		if ( *p++ == old_addr )
		{
			p[ -1 ] = new_addr;
		}
	}
}

static
void get_options( int argc : __D0, char** argv : __A0 )
{
	if ( argc > 1  &&  strcmp( argv[ 1 ], "--continue-on-bus-error" ) == 0 )
	{
		continue_on_bus_error = true;
	}
}

int asm main( int argc, char** argv )
{
	LINK     A6,#0
	
	MOVE.L   8(A6),D0   // argc
	MOVEA.L  12(A6),A0  // argv
	JSR      get_options
	
	JSR      set_trace_handler
	BMI.S    bail
	
	JSR      install_Debugger
	JSR      reassign_unimplemented_traps
	
	JSR      0xFFFFFFF8  // module-suspend
	
	// not reached
	
bail:
	MOVEQ.L  #1,D0
	UNLK     A6
	RTS
}
