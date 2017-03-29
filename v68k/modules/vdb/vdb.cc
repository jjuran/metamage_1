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


void* toolbox_trap_table[] : 3 * 1024;

#define TBTRAP( Routine )  (toolbox_trap_table[ _ ## Routine & 0x03FF ] = &Routine ## _patch)

static void install_Debugger()
{
	TBTRAP( Debugger );  // A9FF
	TBTRAP( DebugStr );  // ABFF
}

int asm main( int argc, char** argv )
{
	LINK     A6,#0
	
	JSR      set_trace_handler
	BMI.S    bail
	
	JSR      install_Debugger
	
	JSR      0xFFFFFFF8  // module-suspend
	
	// not reached
	
bail:
	MOVEQ.L  #1,D0
	UNLK     A6
	RTS
}
