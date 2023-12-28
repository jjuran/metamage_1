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

// iota
#include "iota/char_types.hh"

// command
#include "command/get_option.hh"

// gear
#include "gear/parse_decimal.hh"

// log-of-war
#include "logofwar/report.hh"

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
#include "PRAM.hh"
#include "SysError.hh"
#include "VBL.hh"
#include "Zones.hh"
#include "cursor-core.hh"
#include "interrupt-handlers.hh"
#include "memwatch.hh"
#include "options.hh"


#pragma exceptions off


#define STR_LEN( s )  "" s, (sizeof s - 1)

#define PROGRAM  "ams-core"

#define WARN( msg )  write( STDERR_FILENO, STR_LEN( PROGRAM ": " msg "\n" ) )


enum
{
	Opt_auto_0 = '0',  // Always clear memory in NewPtr and NewHandle
	Opt_GNEtix = 'G',  // GetNextEvent throttle, in ticks
	Opt_romgen = 'R',  // ROM generation: 0 for 64K ROM, 1 for 128K, etc.
	Opt_system = 'V',  // System version
	Opt_auto_v = 'Y',  // Auto-sync screen updates to VBL interrupts
	
	Opt_last_byte = 255,
	
	Opt_events_fd,
};

static command::option options[] =
{
	{ "auto-clear",  Opt_auto_0 },
	{ "auto-vsync",  Opt_auto_v },
	
	{ "gne-ticks",  Opt_GNEtix, command::Param_required },
	
	{ "romgen", Opt_romgen, command::Param_required },
	{ "system", Opt_system, command::Param_required },
	
	{ "events-fd", Opt_events_fd, command::Param_required },
	
	NULL,
};

static unsigned romgen;
static unsigned System;


void* UTableBase  : 0x011C;
short UnitNtryCnt : 0x01D2;

void* SysEvtBuf : 0x0146;
QHdr EventQueue : 0x014A;
short SysEvtCnt : 0x0154;
short SysVersion : 0x015A;
Byte  MBState   : 0x0172;

void* SoundPtr  : 0x0262;
short ROM85     : 0x028E;
void* SysZone   : 0x02A6;
void* ApplZone  : 0x02AA;

long  DefltStack : 0x0322;

void* ScrnBase  : 0x0824;
Point RawMouse  : 0x082C;
Point Mouse     : 0x0830;

void* os_trap_table     [] : 1 * 1024;
void* toolbox_trap_table[] : 3 * 1024;

#define OSTRAP( Proc )  (os_trap_table     [ _##Proc & 0x00FF ] = &Proc##_patch)
#define TBTRAP( Proc )  (toolbox_trap_table[ _##Proc & 0x03FF ] = &Proc##_patch)

enum
{
	_ReallocateHandle = _ReallocHandle,
	_ReserveMem       = _ResrvMem,
	_SystemEdit       = _SysEdit,
};


static
void initialize_low_memory_globals()
{
	if ( romgen < 16 )
	{
		ROM85 = 0xFFFFu >> romgen;
	}
	
	SysVersion = System;
	
	DefltStack = 32 * 1024;
	
	/*
		Tetris stores a pointer to a private control block in SoundPtr
		(which otherwise wouldn't get used, since Tetris doesn't use the
		four-tone synthesizer).  At startup, it calls a sound-stopping
		routine that clears a flag byte in that control block, but at that
		time, the control block hasn't been created yet, and SoundPtr still
		contains zero.  To prevent an unmapped access to $0084, initialize
		SoundPtr to point to some harmless landing zone; the screen buffer
		works just fine for this (and the write isn't even visible).
		
		An alternative fix is to write an INIT that patches _LoadSeg so it
		will hot-patch Tetris to avoid the superfluous call to stop sound
		at launch, but this is obviously much trickier and a lot more work.
	*/
	
	SoundPtr = ScrnBase;
	
	MBState = 0x80;
	
	const long initial_mouse_loc = 0x000F000F;  // 15, 15
	
	*(long*) &RawMouse = initial_mouse_loc;
	*(long*) &Mouse    = initial_mouse_loc;
	
	init_lowmem_Cursor();
	init_cursor();
}

static
void initialize_unit_table()
{
	const int n_unit_table_entries = 32;
	const size_t unit_table_bytesize = n_unit_table_entries * sizeof (void**);
	
	UnitNtryCnt = n_unit_table_entries;
	UTableBase  = NewPtrSysClear( unit_table_bytesize );
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
	old_FreeMem = (FreeMem_ProcPtr) os_trap_table[ _FreeMem & 0x00FF ];
	
	OSTRAP( InitZone         );  // A019
	OSTRAP( GetZone          );  // A01A
	OSTRAP( SetZone          );  // A01B
	OSTRAP( FreeMem          );  // A01C
	
	OSTRAP( NewPtr           );  // A11E
	OSTRAP( DisposePtr       );  // A01F
	OSTRAP( SetPtrSize       );  // A020
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
	OSTRAP( InitApplZone     );  // A02C
	OSTRAP( SetApplLimit     );  // A02D
	
	OSTRAP( MoreMasters      );  // A036
	
	OSTRAP( ReserveMem       );  // A040
	
	OSTRAP( HPurge           );  // A049
	OSTRAP( HNoPurge         );  // A04A
	OSTRAP( SetGrowZone      );  // A04B
	OSTRAP( CompactMem       );  // A04C
	OSTRAP( PurgeMem         );  // A04D
	
	OSTRAP( MaxBlock         );  // A061
	OSTRAP( PurgeSpace       );  // A062
	OSTRAP( MaxApplZone      );  // A063
	OSTRAP( MoveHHi          );  // A064
	
	OSTRAP( NewEmptyHandle   );  // A066
	OSTRAP( HSetRBit         );  // A067
	OSTRAP( HClrRBit         );  // A068
	OSTRAP( HGetState        );  // A069
	OSTRAP( HSetState        );  // A06A
	
	OSTRAP( MaxMem           );  // A11D
	
	TBTRAP( Munger      );  // A9E0
	TBTRAP( HandToHand  );  // A9E1
	TBTRAP( PtrToXHand  );  // A9E2
	TBTRAP( PtrToHand   );  // A9E3
	TBTRAP( HandAndHand );  // A9E4
	
	TBTRAP( PtrAndHand  );  // A9EF
}

static
void install_OSUtils()
{
	OSTRAP( WriteParam   );  // A038
	OSTRAP( ReadDateTime );  // A039
	
	OSTRAP( Delay     );  // A03B
	OSTRAP( CmpString );  // A03C
	
	OSTRAP( InitUtil  );  // A03F
	
	OSTRAP( RelString );  // A050
	OSTRAP( ReadXPRam );  // A051
	
	OSTRAP( UprString );  // A054
	
	OSTRAP( SysEnvirons );  // A090
	
	TBTRAP( Enqueue   );  // A96E
	TBTRAP( Dequeue   );  // A96F
	
	TBTRAP( Secs2Date );  // A9C6
	TBTRAP( Date2Secs );  // A9C7
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
	const short n_max_events = 20;
	
	const int event_size = 4 + sizeof (EvQEl);
	const size_t event_buffer_bytesize = n_max_events * event_size;
	
	SysEvtBuf = NewPtrSysClear( event_buffer_bytesize );
	SysEvtCnt = n_max_events - 1;
	
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
	TBTRAP( SystemEvent );  // A9B2
	
	TBTRAP( SystemTask );  // A9B4
	
	TBTRAP( SystemEdit );  // A9C2
}

static
void install_Debugger()
{
	TBTRAP( Debugger );  // A9FF
	TBTRAP( DebugStr );  // ABFF
}

static
unsigned parse_version( const char* version_string )
{
	const char* p = version_string;
	
	char c;
	
	if ( iota::is_digit( c = *p++ ) )
	{
		int version = (c - '0') << 8;
		
		if ( ! (c = *p++) )
		{
			return version;
		}
		
		if ( c == '.'  &&  iota::is_digit( c = *p++ ) )
		{
			version |= (c - '0') << 4;
			
			if ( ! (c = *p++) )
			{
				return version;
			}
			
			if ( c == '.'  &&  iota::is_digit( c = *p++ ) )
			{
				version |= c - '0';
				
				if ( ! (c = *p++) )
				{
					return version;
				}
			}
		}
	}
	
	FATAL = "Invalid version '", version_string, "'";
	
	exit( 1 );
	
	return 0;
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
			case Opt_auto_0:
				autoclear = true;
				break;
			
			case Opt_auto_v:
				auto_vsync = true;
				break;
			
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
			
			case Opt_system:
				System = parse_version( global_result.param );
				break;
			
			default:
				break;
		}
	}
	
	return argv;
}

static
void create_system_heap()
{
	Ptr start = (Ptr) (1024 * 9);
	Ptr limit = (Ptr) (1024 * 12);
	
	SysZone  = start;
	ApplZone = limit;
	
	InitZone( NULL, 32, limit, start );
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
	
	initialize_low_memory_globals();
	
	install_spinloop_mitigation();
	
	install_MemoryManager();
	
	create_system_heap();
	
	install_OSUtils();
	
	set_syserr_handler();
	
	install_VerticalRetraceManager();
	
	install_PatchManager();
	
	install_Gestalt();
	
	install_OSEventManager();
	install_EventManager();
	
	install_DeskManager();
	
	install_Debugger();
	
	initialize_unit_table();
	
	module_suspend();  // doesn't return
}
