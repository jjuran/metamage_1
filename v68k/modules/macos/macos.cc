/*
	macos.cc
	--------
*/

// Mac OS
#ifndef __TRAPS__
#include <Traps.h>
#endif

// POSIX
#include <unistd.h>

// macos
#include "Debugger.hh"
#include "GrafPorts.hh"
#include "Handles.hh"
#include "InitGraf.hh"
#include "OSUtils.hh"
#include "Rects.hh"
#include "Regions.hh"
#include "Segments.hh"


#define STR_LEN( s )  "" s, (sizeof s - 1)


unsigned long ScrnBase : 0x0824;

void* os_trap_table     [] : 1 * 1024;
void* toolbox_trap_table[] : 3 * 1024;

#define OSTRAP( Routine )  (os_trap_table     [ _ ## Routine & 0x00FF ] = &Routine ## _patch)
#define TBTRAP( Routine )  (toolbox_trap_table[ _ ## Routine & 0x03FF ] = &Routine ## _patch)

enum
{
	_ReallocateHandle = _ReallocHandle,
	_SetPortBits      = _SetPBits,
	_SetRectRgn       = _SetRecRgn,
};


static void initialize_low_memory_globals()
{
	ScrnBase = 0x0001A700;
}

static void install_MemoryManager()
{
	OSTRAP( NewHandle        );  // A022
	OSTRAP( DisposeHandle    );  // A023
	OSTRAP( SetHandleSize    );  // A024
	OSTRAP( GetHandleSize    );  // A025
	OSTRAP( ReallocateHandle );  // A027
	OSTRAP( EmptyHandle      );  // A02B
	OSTRAP( NewEmptyHandle   );  // A066
}

static void install_OSUtils()
{
	OSTRAP( Delay     );  // A03B
	TBTRAP( TickCount );  // A975
	TBTRAP( SysBeep   );  // A9C8
}

static void install_QuickDraw()
{
	TBTRAP( InitPort  );  // A86D
	TBTRAP( InitGraf  );  // A86E
	TBTRAP( OpenPort  );  // A86F
	TBTRAP( SetPort   );  // A873
	TBTRAP( GetPort   );  // A874
	TBTRAP( ClosePort );  // A87D
	
	TBTRAP( SetPortBits );  // A875
	
	TBTRAP( FrameRect  );  // A8A1
	TBTRAP( PaintRect  );  // A8A2
	TBTRAP( EraseRect  );  // A8A3
	TBTRAP( InverRect  );  // A8A4
	TBTRAP( FillRect   );  // A8A5
	TBTRAP( EqualRect  );  // A8A6
	TBTRAP( SetRect    );  // A8A7
	TBTRAP( OffsetRect );  // A8A8
	TBTRAP( InsetRect  );  // A8A9
	
	TBTRAP( EmptyRect  );  // A8AE
	
	TBTRAP( NewRgn      );  // A8D8
	TBTRAP( DisposeRgn  );  // A8D9
	TBTRAP( SetEmptyRgn );  // A8DD
	TBTRAP( SetRectRgn  );  // A8DE
	TBTRAP( RectRgn     );  // A8DF
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

static asm void module_suspend()
{
	JSR      0xFFFFFFF8
}

#define NOT_A_PROGRAM  "macos: Not a program; use `xv68k -m macos ...` to load as a module\n"

int main( int argc, char** argv )
{
	initialize_low_memory_globals();
	
	install_MemoryManager();
	
	install_OSUtils();
	
	install_QuickDraw();
	
	install_SegmentLoader();
	
	install_Debugger();
	
	if ( argc == 0 )
	{
		module_suspend();  // doesn't return
	}
	
	write( STDERR_FILENO, STR_LEN( NOT_A_PROGRAM ) );
	
	return 2;
}
