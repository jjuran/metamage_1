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

// command
#include "command/get_option.hh"

// macos
#include "Debugger.hh"
#include "Fonts.hh"
#include "GrafPorts.hh"
#include "Handles.hh"
#include "InitGraf.hh"
#include "Menus.hh"
#include "OSUtils.hh"
#include "Pen.hh"
#include "Rects.hh"
#include "Region-ops.hh"
#include "Regions.hh"
#include "Segments.hh"
#include "Windows.hh"
#include "options.hh"


#define STR_LEN( s )  "" s, (sizeof s - 1)


enum
{
	Opt_linger = 'L',  // linger on ExitToShell
	Opt_qdsync = 'Q',
};

static command::option options[] =
{
	{ "linger",  Opt_linger },
	{ "qd-sync", Opt_qdsync },
};


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
	TBTRAP( InitPort    );  // A86D
	TBTRAP( InitGraf    );  // A86E
	TBTRAP( OpenPort    );  // A86F
	TBTRAP( SetPort     );  // A873
	TBTRAP( GetPort     );  // A874
	TBTRAP( SetPortBits );  // A875
	TBTRAP( PortSize    );  // A876
	TBTRAP( MovePortTo  );  // A877
	
	TBTRAP( SetClip     );  // A879
	TBTRAP( GetClip     );  // A87A
	TBTRAP( ClipRect    );  // A87B
	TBTRAP( BackPat     );  // A87C
	TBTRAP( ClosePort   );  // A87D
	
	TBTRAP( StdLine );  // A890
	TBTRAP( LineTo  );  // A891
	TBTRAP( Line    );  // A892
	TBTRAP( MoveTo  );  // A893
	TBTRAP( Move    );  // A894
	
	TBTRAP( HidePen     );  // A896
	TBTRAP( ShowPen     );  // A897
	TBTRAP( GetPenState );  // A898
	TBTRAP( SetPenState );  // A899
	TBTRAP( GetPen      );  // A89A
	TBTRAP( PenSize     );  // A89B
	TBTRAP( PenMode     );  // A89C
	TBTRAP( PenPat      );  // A89D
	TBTRAP( PenNormal   );  // A89E
	
	TBTRAP( StdRect    );  // A8A0
	TBTRAP( FrameRect  );  // A8A1
	TBTRAP( PaintRect  );  // A8A2
	TBTRAP( EraseRect  );  // A8A3
	TBTRAP( InverRect  );  // A8A4
	TBTRAP( FillRect   );  // A8A5
	TBTRAP( EqualRect  );  // A8A6
	TBTRAP( SetRect    );  // A8A7
	TBTRAP( OffsetRect );  // A8A8
	TBTRAP( InsetRect  );  // A8A9
	TBTRAP( SectRect   );  // A8AA
	TBTRAP( UnionRect  );  // A8AB
	TBTRAP( Pt2Rect    );  // A8AC
	TBTRAP( PtInRect   );  // A8AD
	TBTRAP( EmptyRect  );  // A8AE
	
	TBTRAP( BitMapToRegion );  // A8D7
	
	TBTRAP( NewRgn      );  // A8D8
	TBTRAP( DisposeRgn  );  // A8D9
	TBTRAP( CopyRgn     );  // A8DC
	TBTRAP( SetEmptyRgn );  // A8DD
	TBTRAP( SetRectRgn  );  // A8DE
	TBTRAP( RectRgn     );  // A8DF
	TBTRAP( OffsetRgn   );  // A8E0
	TBTRAP( EmptyRgn    );  // A8E2
	TBTRAP( SectRgn     );  // A8E4
	TBTRAP( UnionRgn    );  // A8E5
	TBTRAP( DiffRgn     );  // A8E6
	TBTRAP( XOrRgn      );  // A8E7
}

static void install_Fonts()
{
	TBTRAP( InitFonts );  // A8FE
}

static void install_Windows()
{
	TBTRAP( InitWindows );  // A912
}

static void install_Menus()
{
	TBTRAP( InitMenus );  // A930
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

static char* const* get_options( char** argv )
{
	int opt;
	
	++argv;  // skip arg 0
	
	while ( (opt = command::get_option( (char* const**) &argv, options )) > 0 )
	{
		switch ( opt )
		{
			case Opt_linger:
				linger_on_exit = true;
				break;
			
			case Opt_qdsync:
				synchronized_quickdraw = true;
				break;
			
			default:
				break;
		}
	}
	
	return argv;
}

int main( int argc, char** argv )
{
	if ( argc > 0 )
	{
		char* const* args = get_options( argv );
		
		if ( *args != NULL )
		{
			write( STDERR_FILENO, STR_LEN( "macos: Warning: ignored non-option arguments\n" ) );
		}
	}
	
	initialize_low_memory_globals();
	
	install_MemoryManager();
	
	install_OSUtils();
	
	install_QuickDraw();
	install_Fonts();
	install_Windows();
	install_Menus();
	
	install_SegmentLoader();
	
	install_Debugger();
	
	module_suspend();  // doesn't return
}
