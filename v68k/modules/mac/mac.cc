/*
	mac.cc
	------
*/

// Mac OS
#ifndef __TRAPS__
#include <Traps.h>
#endif

// Standard C
#include <errno.h>

// Relix
#include "tool-runtime/parameter_block.h"

// mac
#include "Debugger.hh"
#include "InitGraf.hh"
#include "OSUtils.hh"
#include "Rects.hh"
#include "Segments.hh"


void* os_trap_table     [] : 1 * 1024;
void* toolbox_trap_table[] : 3 * 1024;

#define OSTRAP( Routine )  (os_trap_table     [ _ ## Routine & 0x00FF ] = &Routine ## _patch)
#define TBTRAP( Routine )  (toolbox_trap_table[ _ ## Routine & 0x03FF ] = &Routine ## _patch)

static void install_OSUtils()
{
	OSTRAP( Delay     );  // A03B
	TBTRAP( TickCount );  // A975
	TBTRAP( SysBeep   );  // A9C8
}

static void install_QuickDraw()
{
	TBTRAP( InitGraf );  // A86E
	
	TBTRAP( FrameRect );  // A8A1
	TBTRAP( PaintRect );  // A8A2
	TBTRAP( EraseRect );  // A8A3
	TBTRAP( InverRect );  // A8A4
	TBTRAP( FillRect  );  // A8A5
}

static void install_SegmentLoader()
{
	TBTRAP( ExitToShell );  // A9F4
}

static void install_Debugger()
{
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

typedef int (*main_entry)(int, char**, char**, void*);

int main( int argc, char** argv )
{
	if ( argc < 2 )
	{
		return 0;
	}
	
	install_OSUtils();
	
	install_QuickDraw();
	
	install_SegmentLoader();
	
	install_Debugger();
	
	const char* path = argv[1];
	
	void* program = load( path );
	
	if ( program == 0 )  // NULL
	{
		return errno == ENOENT ? 127 : 126;
	}
	
	main_entry next = (main_entry) program;
	
	--argc;
	++argv;
	
	return next( argc, argv, 0, global_system_params );  // NULL
}

