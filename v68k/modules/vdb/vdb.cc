/*
	vdb.cc
	------
*/

// Mac OS
#ifndef __TRAPS__
#include <Traps.h>
#endif

// vdb
#include "Debugger.hh"
#include "trace.hh"


void* os_trap_table     [] : 1 * 1024;
void* toolbox_trap_table[] : 3 * 1024;

#define TBTRAP( Routine )  (toolbox_trap_table[ _ ## Routine & 0x03FF ] = &Routine ## _patch)

static void install_Debugger()
{
	TBTRAP( Debugger );  // A9FF
	TBTRAP( DebugStr );  // ABFF
}

static
void reassign_unimplemented_traps()
{
	void* old_addr = toolbox_trap_table[ _Unimplemented & 0x03FF ];
	void* new_addr = &user_break;
	
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

int asm main( int argc, char** argv )
{
	LINK     A6,#0
	
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
