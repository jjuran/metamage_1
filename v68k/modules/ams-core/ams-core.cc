/*
	ams-core.cc
	-----------
*/

// Mac OS
#ifndef __TRAPS__
#include <Traps.h>
#endif
#ifndef __EVENTS__
#include <Events.h>
#endif

// POSIX
#include <unistd.h>

// Standard C
#include <stdlib.h>

// command
#include "command/get_option.hh"

// gear
#include "gear/parse_decimal.hh"

// ams-common
#include "callouts.hh"

// ams-core
#include "Debugger.hh"
#include "Desk.hh"
#include "Events.hh"
#include "Gestalt.hh"
#include "HandleUtils.hh"
#include "Handles.hh"
#include "OSEvents.hh"
#include "OSUtils.hh"
#include "Patches.hh"
#include "Pointers.hh"
#include "SysError.hh"
#include "VBL.hh"
#include "cursor-core.hh"
#include "interrupt-handlers.hh"
#include "memwatch.hh"
#include "options.hh"


#define STR_LEN( s )  "" s, (sizeof s - 1)

#define PROGRAM  "ams-core"

#define WARN( msg )  write( STDERR_FILENO, STR_LEN( PROGRAM ": " msg "\n" ) )


enum
{
	Opt_GNEtix = 'G',  // GetNextEvent throttle, in ticks
	Opt_linger = 'L',  // linger on ExitToShell
	Opt_romgen = 'R',  // ROM generation: 0 for 64K ROM, 1 for 128K, etc.
	
	Opt_last_byte = 255,
	
	Opt_events_fd,
};

static command::option options[] =
{
	{ "gne-ticks",  Opt_GNEtix, command::Param_required },
	
	{ "linger",  Opt_linger },
	
	{ "romgen", Opt_romgen, command::Param_required },
	
	{ "events-fd", Opt_events_fd, command::Param_required },
	
	NULL,
};

static unsigned romgen;


void* UTableBase  : 0x011C;
short UnitNtryCnt : 0x01D2;

void* SysEvtBuf : 0x0146;
QHdr EventQueue : 0x014A;
short SysEvtCnt : 0x0154;
Byte  MBState   : 0x0172;

short ROM85     : 0x028E;

long  DefltStack : 0x0322;

short ScreenRow : 0x0106;
void* ScrnBase  : 0x0824;
Point Mouse     : 0x0830;
Rect  CrsrPin   : 0x0834;

void* os_trap_table     [] : 1 * 1024;
void* toolbox_trap_table[] : 3 * 1024;

#define OSTRAP( Proc )  (os_trap_table     [ _##Proc & 0x00FF ] = &Proc##_patch)
#define TBTRAP( Proc )  (toolbox_trap_table[ _##Proc & 0x03FF ] = &Proc##_patch)

enum
{
	_ReallocateHandle = _ReallocHandle,
	_ReserveMem       = _ResrvMem,
};


static inline
pascal asm void ScrnBitMap( BitMap* screenBits )
{
	DC.W     _ScrnBitMap
}

static
void initialize_low_memory_globals()
{
	if ( romgen < 16 )
	{
		ROM85 = 0xFFFFu >> romgen;
	}
	
	DefltStack = 64 * 1024;
	
	const int n_unit_table_entries = 32;
	
	UnitNtryCnt = n_unit_table_entries;
	UTableBase  = calloc( n_unit_table_entries, sizeof (void**) );
	
	BitMap screenBits;
	
	ScrnBitMap( &screenBits );
	
	ScreenRow = screenBits.rowBytes;
	
	const short n_max_events = 20;
	
	const int event_size = 4 + sizeof (EvQEl);
	
	SysEvtBuf = calloc( event_size, n_max_events );
	SysEvtCnt = n_max_events - 1;
	
	MBState = 0x80;
	
	ScrnBase = screenBits.baseAddr;
	
	*(long*) &Mouse = 0x000F000F;  // 15, 15
	
	CrsrPin = screenBits.bounds;
	
	init_lowmem_Cursor();
	init_cursor();
}

static
void install_spinloop_mitigation()
{
	enum
	{
		_WaitUntilZero = 0xA0F6,
	};
	
	OSTRAP( WaitUntilZero );  // A0F6
}

static
void install_MemoryManager()
{
	OSTRAP( FreeMem          );  // A01C
	
	OSTRAP( NewPtr           );  // A11E
	OSTRAP( DisposePtr       );  // A01F
	
	OSTRAP( GetPtrSize       );  // A021
	OSTRAP( NewHandle        );  // A022
	OSTRAP( DisposeHandle    );  // A023
	OSTRAP( SetHandleSize    );  // A024
	OSTRAP( GetHandleSize    );  // A025
	OSTRAP( ReallocateHandle );  // A027
	OSTRAP( RecoverHandle    );  // A028
	OSTRAP( HLock            );  // A029
	OSTRAP( HUnlock          );  // A02A
	OSTRAP( EmptyHandle      );  // A02B
	
	OSTRAP( SetApplLimit     );  // A02D
	
	OSTRAP( MoreMasters      );  // A036
	
	OSTRAP( ReserveMem       );  // A040
	
	OSTRAP( HPurge           );  // A049
	OSTRAP( HNoPurge         );  // A04A
	OSTRAP( SetGrowZone      );  // A04B
	OSTRAP( CompactMem       );  // A04C
	OSTRAP( PurgeMem         );  // A04D
	
	OSTRAP( MaxApplZone      );  // A063
	OSTRAP( MoveHHi          );  // A064
	
	OSTRAP( NewEmptyHandle   );  // A066
	
	OSTRAP( HGetState        );  // A069
	OSTRAP( HSetState        );  // A06A
	
	OSTRAP( MaxMem           );  // A11D
	
	TBTRAP( HandToHand  );  // A9E1
	TBTRAP( PtrToXHand  );  // A9E2
	TBTRAP( PtrToHand   );  // A9E3
}

static
void install_OSUtils()
{
	OSTRAP( ReadDateTime );  // A039
	
	OSTRAP( Delay     );  // A03B
	OSTRAP( CmpString );  // A03C
	
	OSTRAP( UprString );  // A054
	
	OSTRAP( SysEnvirons );  // A090
	
	TBTRAP( Enqueue   );  // A96E
	TBTRAP( Dequeue   );  // A96F
	
	TBTRAP( Secs2Date );  // A9C6
	
	TBTRAP( SysBeep   );  // A9C8
	TBTRAP( SysError  );  // A9C9
}

static
void install_VerticalRetraceManager()
{
	OSTRAP( VInstall );  // A033
	OSTRAP( VRemove  );  // A034
}

static
void install_PatchManager()
{
	OSTRAP( GetTrapAddress );  // A146
	OSTRAP( SetTrapAddress );  // A047
}

static
void install_Gestalt()
{
	old_Gestalt = (Gestalt_ProcPtr) os_trap_table[ _Gestalt & 0x00FF ];
	
	OSTRAP( Gestalt );  // A1AD
}

static
void install_OSEventManager()
{
	OSTRAP( PostEvent    );  // A02F
	OSTRAP( OSEventAvail );  // A030
	OSTRAP( GetOSEvent   );  // A031
	OSTRAP( FlushEvents  );  // A032
	
	install_interrupt_handlers();
}

static
void install_EventManager()
{
	TBTRAP( WaitNextEvent );  // A860
	TBTRAP( GetNextEvent  );  // A970
	TBTRAP( EventAvail    );  // A971
	TBTRAP( GetMouse      );  // A972
	TBTRAP( StillDown     );  // A973
	TBTRAP( Button        );  // A974
	TBTRAP( TickCount     );  // A975
	TBTRAP( GetKeys       );  // A976
	TBTRAP( WaitMouseUp   );  // A977
}

static
void install_DeskManager()
{
	TBTRAP( SystemTask );  // A9B4
}

static
void install_Debugger()
{
	TBTRAP( Debugger );  // A9FF
	TBTRAP( DebugStr );  // ABFF
}

static
char* const* get_options( char** argv )
{
	using command::global_result;
	
	int opt;
	
	++argv;  // skip arg 0
	
	while ( (opt = command::get_option( (char* const**) &argv, options )) > 0 )
	{
		switch ( opt )
		{
			case Opt_events_fd:
				events_fd = gear::parse_unsigned_decimal( global_result.param );
				break;
			
			case Opt_GNEtix:
				GetNextEvent_throttle =
					gear::parse_unsigned_decimal( global_result.param );
				break;
			
			case Opt_romgen:
				romgen = gear::parse_unsigned_decimal( global_result.param );
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
			WARN( "no arguments allowed" );
			
			_exit( 1 );
		}
	}
	
	set_syserr_handler();
	
	initialize_low_memory_globals();
	
	install_spinloop_mitigation();
	
	install_MemoryManager();
	
	install_OSUtils();
	
	install_VerticalRetraceManager();
	
	install_PatchManager();
	
	install_Gestalt();
	
	install_OSEventManager();
	install_EventManager();
	
	install_DeskManager();
	
	install_Debugger();
	
	module_suspend();  // doesn't return
}
