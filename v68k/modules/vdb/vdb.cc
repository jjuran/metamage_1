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
#include <errno.h>

// Relix
#include "tool-runtime/parameter_block.h"

// v68k-utils
#include "utils/load.hh"

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

using v68k::utils::load;

int asm main( int argc, char** argv )
{
	LINK     A6,#0
	
	MOVEQ.L  #0,D0
	SUBQ.L   #1,8(A6)
	BLE.S    exit
	
	JSR      set_trace_handler
	BMI.S    bail
	
	JSR      install_Debugger
	
	ADDQ.L   #4,12(A6)
	MOVEA.L  12(A6),A0
	MOVEA.L  (A0),A0
	
	JSR      load
	MOVE.L   A0,D0
	BNE.S    loaded
	
	CMPI.L   #ENOENT,D1
	SNE      D0
	ADDI.B   #127,D0
	BRA.S    exit
	
loaded:
	MOVE.L   global_system_params,-(SP)
	CLR.L    -(SP)
	MOVE.L   12(A6),-(SP)
	MOVE.L   8(A6),-(SP)
	MOVEA.L  A0,A1
	
	JSR      0xFFFFFFF4
	
	JSR      (A1)
	
	JSR      0xFFFFFFF0
	
	MOVE.L   D0,-(SP)
	JSR      _exit
	// not reached
	
bail:
	MOVEQ.L  #1,D0
exit:
	UNLK     A6
	RTS
}
