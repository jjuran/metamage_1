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

// mac
#include "Debugger.hh"
#include "trace.hh"


void* toolbox_trap_table[] : 3 * 1024;

#define TBTRAP( Routine )  (toolbox_trap_table[ _ ## Routine & 0x03FF ] = &Routine ## _patch)

static void install_Debugger()
{
	set_trace_handler();
	
	TBTRAP( Debugger );  // A9FF
	TBTRAP( DebugStr );  // ABFF
}

static asm void* load( const char* path : __A0 ) : __A0
{
	MOVEA.L  A0,A1    ; // copy A0
	
	// ENTER strlen0
loop:
	TST.B    (A1)+    ;  // while ( *a1++ != '\0' )
	BNE.S    loop     ;  //    continue;
	
	SUBA     A0,A1    ;  // a1 -= a0;
	MOVE.L   A1,D0    ;  // d0 = a1;
	// LEAVE strlen0
	
	JSR  0xFFFFFFFC   ;  // the actual load callback
	
	MOVE.L   A0,D2    ;  // if ( *a0 != NULL ) ;
	BNE.S    no_errno ;  // else
	MOVE.L   D1,errno ;  //     errno = d1;
no_errno:
	RTS
}

int asm main( int argc, char** argv )
{
	LINK     A6,#0
	
	SUBQ.L   #1,8(A6)
	BLE.S    bail
	
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
	MOVEQ.L  #0,D0
exit:
	UNLK     A6
	RTS
}

