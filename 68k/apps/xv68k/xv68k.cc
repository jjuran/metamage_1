/*
	xv68k.cc
	--------
*/

// Standard C
#include <signal.h>
#include <stdlib.h>
#include <string.h>

// POSIX
#include <fcntl.h>
#include <unistd.h>

// more-libc
#include "more/string.h"

// more-posix
#include "more/perror.hh"

// gear
#include "gear/inscribe_decimal.hh"
#include "gear/parse_decimal.hh"

// log-of-war
#include "logofwar/report.hh"

// command
#include "command/get_option.hh"

// v68k
#include "v68k/emulator.hh"
#include "v68k/endian.hh"
#include "v68k/print.hh"

// v68k-alloc
#include "v68k-alloc/memory.hh"

// v68k-auth
#include "auth/auth.hh"

// v68k-mac
#include "v68k-mac/memory.hh"
#include "v68k-mac/trap_dispatcher.hh"

// v68k-user
#include "v68k-user/load.hh"

// v68k-callouts
#include "callout/bridge.hh"

// v68k-utils
#include "utils/load.hh"
#include "utils/print_register_dump.hh"

// v68k-screen
#include "screen/lock.hh"
#include "screen/storage.hh"
#include "screen/surface.hh"

// v68k-syscalls
#include "syscall/bridge.hh"
#include "syscall/handler.hh"

// xv68k
#include "cursor.hh"
#include "diagnostics.hh"
#include "memory.hh"
#include "native.hh"
#include "screen.hh"


#ifdef __MWERKS__
#pragma exceptions off
#endif


#define STR_LEN( s )  "" s, (sizeof s - 1)


using v68k::big_longword;

using v68k::auth::fully_authorized;
using v68k::auth::supervisor_mode_switch_allowed;
using v68k::callout::callout_address;
using v68k::callout::system_call;
using v68k::callout::microseconds;
using v68k::screen::ignore_screen_locks;

enum
{
	/*
		The presence of this value in CrsrSave at startup
		indicates to ams-core that a hardware cursor exists.
	*/
	
	kHardwareCursor = 0x68647772,  // 'hdwr'
};

static v68k::processor_model mc68k_model = v68k::mc68000;

static bool turbo;
static bool polling;
static bool tracing;
static bool verbose;
static bool has_screen;
static bool has_cursor;

static unsigned long n_instructions;

static int n_512K_blocks = -1;

struct module_spec
{
	const char*  name;
	
	char* const*  begin;
	char* const*  end;
};

static module_spec* module_specs;


enum
{
	Opt_authorized = 'A',
	Opt_model      = 'M',
	Opt_poll       = 'P',
	Opt_supervisor = 'S',
	Opt_trace      = 'T',
	Opt_module     = 'm',
	Opt_turbo      = 't',
	Opt_verbose    = 'v',
	
	Opt_last_byte = 255,
	
	Opt_mem,  // number of 512K memory chunks, or zero for 128K
	Opt_pid,
	Opt_raster,
	Opt_cursor,
	Opt_ignore_screen_locks,
};

static command::option options[] =
{
	{ "",           Opt_authorized },
	{ "poll",       Opt_poll       },
	{ "supervisor", Opt_supervisor },
	{ "trace",      Opt_trace      },
	{ "turbo",      Opt_turbo      },
	{ "verbose",    Opt_verbose    },
	{ "mem",        Opt_mem,    command::Param_required },
	{ "model",      Opt_model,  command::Param_required },
	{ "pid",        Opt_pid,    command::Param_optional },
	{ "raster",     Opt_raster, command::Param_required },
	{ "cursor",     Opt_cursor, command::Param_required },
	{ "module",     Opt_module, command::Param_required },
	
	{ "ignore-screen-locks", Opt_ignore_screen_locks },
	
	{ NULL }
};


static sig_atomic_t sigint_pending;

static
void sigint_handler( int )
{
	sigint_pending = true;
}

#define EXIT( status, msg )  exit_with_message( STR_LEN( msg "\n" ), status )

static
void exit_with_message( const char* msg, unsigned len, int status )
{
	write( STDERR_FILENO, msg, len );
	
	exit( status );
}

static
void atexit_report()
{
	if ( verbose )
	{
		const char* count = gear::inscribe_unsigned_decimal( n_instructions );
		
		write( STDERR_FILENO, STR_LEN( "### Instruction count: " ) );
		write( STDERR_FILENO, count, strlen( count ) );
		write( STDERR_FILENO, STR_LEN( "\n" ) );
	}
}

static
void dump( const v68k::processor_state& s )
{
	using v68k::utils::print_register_dump;
	
	print_register_dump( s.regs, s.get_SR() );
}

static
void dump_and_raise( const v68k::processor_state& s, int signo )
{
	dump( s );
	
	atexit_report();
	
	raise( signo );
}

/*
	Memory map
	----------
	
	0K	+-----------------------+
		| System vectors        |  1K
	1K	+-----------------------+
		| OS trap table         |  1K
	2K	+-----------------------+
		| supervisor stack      |  1K
	3K	+-----------------------+
		|                       |
	4K	|                       |
		|                       |
		| Toolbox trap table    |
		|                       |
		|                       |
		|                       |  4K
	7K	+-----------------------+
		| boot code / OS        |  1K
	8K	+-----------------------+
		|                       |
		| argc/argv/envp params |
		|                       |  2K
	10K	+-----------------------+
		|                       |
		|                       |
		|                       |
		|                       |
		| user stack            =  x4
		|                       |
		|                       |
		|                       |
		|                       |  20K
	30K	+-----------------------+
		|                       |
		|                       |
		|                       |
		|                       |
		|                       |
		|                       |
		|                       |
		= user code             =  x5
		|                       |
		|                       |
		|                       |
		|                       |
		|                       |
		|                       |
		|                       |  40K
	70K	+-----------------------+
		
	72K	+-----------------------+
		|                       |  screen memory begins 0x00012700  (1792 after)
		|                       |  screen memory ends   0x00017C80  (896 before)
		|                       |
		=   alt screen buffer   =  x6
		|                       |
		|                       |
		|                       |  24K (21.375K used)
	96K	+-----------------------+
		
	104	+-----------------------+
		|                       |  screen memory begins 0x0001A700  (1792 after)
		|                       |  screen memory ends   0x0001FC80  (896 before)
		|                       |
		=  main screen buffer   =  x6
		|                       |
		|                       |
		|                       |  24K (21.375K used)
	128	+-----------------------+
	
*/

const uint32_t params_max_size = 2048;
const uint32_t code_max_size   = 40 * 1024;

const uint32_t os_address   = 7168;
const uint32_t initial_SSP  = 3072;
const uint16_t stack_limit  = 10240;
const uint16_t initial_USP  = 30720;
const uint32_t code_address = 30720;

const uint32_t os_trap_table_address = 1024;
const uint32_t tb_trap_table_address = 3072;

const uint32_t os_trap_count = 1 <<  8;  //  256, 1K
const uint32_t tb_trap_count = 1 << 10;  // 1024, 4K

const uint32_t params_addr = 8192;

const uint16_t user_pb_addr   = params_addr +  0;  // 20 bytes
const uint16_t system_pb_addr = params_addr + 20;  // 20 bytes

const uint16_t user_pb_stack_bottom_addr = user_pb_addr + 0;
const uint16_t user_pb_stack_limit_addr  = user_pb_addr + 4;

const uint16_t argc_addr = params_addr + 40;  // 4 bytes
const uint16_t argv_addr = params_addr + 44;  // 4 bytes
const uint32_t args_addr = params_addr + 48;

static uint32_t mem_size = 1024 * 128;


static
void init_trap_table( uint32_t* table, uint32_t* end, uint32_t address )
{
	const uint32_t unimplemented = v68k::big_longword( address );
	
	for ( uint32_t* p = table;  p < end;  ++p )
	{
		*p = unimplemented;
	}
}


static
const uint16_t loader_code[] =
{
	0x027C,  // ANDI #0xD8FF,SR  ; clear S and the interrupt mask
	0xD8FF,
	
	0x4FF8,  // LEA  (3072).W,A7
	initial_USP,
	
	0x4BF8,  // LEA  (12288).W,A5
	0x3000,
	
	0x21CF,  // MOVE.L   A7,(user_pb_stack_bottom_addr).W
	user_pb_stack_bottom_addr,
	
	0x21FC,  // MOVE.L  #stack_limit,(user_pb_stack_limit_addr).W
	0x0000,
	stack_limit,
	user_pb_stack_limit_addr,
	
	0x42B8,  // CLR.L  user_pb_addr + 2 * sizeof (u32)  ; user_pb->errno_var
	user_pb_addr + 2 * sizeof (uint32_t),
	
	0x21FC,  // MOVE.L  #user_pb_addr,(system_pb_addr).W  ; pb->current_user
	0x0000,
	user_pb_addr,
	system_pb_addr,
	
	0x21FC,  // MOVE.L   #callout_address(system_call),<dispatcher field>
	0xFFFF,
	(uint16_t) callout_address(system_call),
	system_pb_addr + 3 * sizeof (uint32_t),
	
	0x21FC,  // MOVE.L   #callout_address(microseconds),<microseconds field>
	0xFFFF,
	(uint16_t) callout_address(microseconds),
	system_pb_addr + 4 * sizeof (uint32_t),
	
	0x4878,  // PEA  (system_pb_addr).W  ; pb
	system_pb_addr,
	
	0x4878,  // PEA  (0).W  ; envp
	0x0000,
	
	0x2F38,  // MOVE.L  (argv_addr).W,-(A7)
	argv_addr,
	
	0x2F38,  // MOVE.L  (argc_addr).W,-(A7)
	argc_addr,
	
	0x4EB8,  // JSR  code_address
	code_address,
	
	0x2F00,  // MOVE.L  D0,-(A7)
	0x42A7,  // CLR.L   -(A7)  ; zero return address
	0x7001,  // MOVEQ  #1,D0
	0x484A,  // BKPT   #2
	
	// Not reached
	0x4E75   // RTS
};

static
const uint16_t no_op_exception[] =
{
	0x4E73,  // RTE
};

#define HANDLER( handler )  handler, sizeof handler

static
void load_vectors( v68k::user::os_load_spec& os )
{
	uint32_t* vectors = (uint32_t*) os.mem_base;
	
	memset( vectors, 0xFF, 1024 );
	
	vectors[0] = big_longword( initial_SSP );  // isp
	
	using v68k::user::install_exception_handler;
	using v68k::mac::trap_dispatcher;
	
	install_exception_handler( os,  1, HANDLER( loader_code ) );
	install_exception_handler( os, 10, HANDLER( trap_dispatcher ) );
	install_exception_handler( os, 32 + 2, HANDLER( syscall_patch_handler ) );
	
	install_exception_handler( os, 64, HANDLER( no_op_exception ) );
	install_exception_handler( os, 88, HANDLER( no_op_exception ) );
	
	using namespace v68k::callout;
	
	vectors[ 2] = big_longword( callout_address( bus_error           ) );
	vectors[ 3] = big_longword( callout_address( address_error       ) );
	vectors[ 4] = big_longword( callout_address( illegal_instruction ) );
	vectors[ 5] = big_longword( callout_address( division_by_zero    ) );
	vectors[ 6] = big_longword( callout_address( chk_trap            ) );
	vectors[ 7] = big_longword( callout_address( trapv_trap          ) );
	vectors[ 8] = big_longword( callout_address( privilege_violation ) );
	vectors[ 9] = big_longword( callout_address( trace_exception     ) );
	vectors[11] = big_longword( callout_address( line_F_emulator     ) );
	vectors[14] = big_longword( callout_address( format_error        ) );
	
	vectors[66] = big_longword( callout_address( sigint_interrupt    ) );
}

static
void load_Mac_traps( uint8_t* mem )
{
	using namespace v68k::callout;
	
	uint32_t* os_traps = (uint32_t*) &mem[ os_trap_table_address ];
	uint32_t* tb_traps = (uint32_t*) &mem[ tb_trap_table_address ];
	
	const uint32_t unimplemented = callout_address( unimplemented_trap );
	
	init_trap_table( os_traps, os_traps + os_trap_count, unimplemented );
	init_trap_table( tb_traps, tb_traps + tb_trap_count, unimplemented );
	
	os_traps[ 0x1C ] = big_longword( callout_address( FreeMem_trap     ) );
	os_traps[ 0x1E ] = big_longword( callout_address( alloc            ) );
	os_traps[ 0x1F ] = big_longword( callout_address( dealloc          ) );
	os_traps[ 0x2E ] = big_longword( callout_address( BlockMove_trap   ) );
	os_traps[ 0x93 ] = big_longword( callout_address( get_microseconds ) );
	os_traps[ 0xAD ] = big_longword( callout_address( Gestalt_trap     ) );
	
	const uint32_t big_no_op = big_longword( callout_address( no_op ) );
	
	os_traps[ 0x46 ] = big_no_op;  // GetTrapAddress
	os_traps[ 0x47 ] = big_no_op;  // SetTrapAddress
	os_traps[ 0x55 ] = big_no_op;  // StripAddress
	os_traps[ 0x98 ] = big_no_op;  // HWPriv
}

static
void load_argv( uint8_t* mem, int argc, char* const* argv )
{
	(uint32_t&) mem[ argc_addr ] = big_longword( argc      );
	(uint32_t&) mem[ argv_addr ] = big_longword( args_addr );
	
	uint32_t* args = (uint32_t*) &mem[ args_addr ];
	
	uint8_t* args_limit = &mem[ params_addr ] + params_max_size;
	
	uint8_t* args_data = (uint8_t*) (args + argc + 1);
	
	if ( args_data >= args_limit )
	{
		err_argv_way_too_big();
		
		abort();
	}
	
	while ( *argv != NULL )
	{
		*args++ = big_longword( args_data - mem );
		
		const ssize_t len = strlen( *argv ) + 1;
		
		if ( len > args_limit - args_data )
		{
			err_argv_too_big();
			
			abort();
		}
		
		args_data = (uint8_t*) mempcpy( args_data, *argv++, len );
	}
	
	*args = 0;  // trailing NULL of argv
}

static
void load_file( uint8_t* mem, const char* path );

static
void load_code( uint8_t* mem, const char* path )
{
	int fd;
	
	if ( path == NULL  ||  (path[0] == '-'  &&  path[1] == '\0') )
	{
		fd = STDIN_FILENO;
	}
	else
	{
		load_file( mem, path );
		
		return;
	}
	
	ssize_t n_read = read( fd, mem + code_address, code_max_size );
	
	if ( n_read < 0 )
	{
		abort();
	}
	
	if ( n_read == 0 )
	{
		EXIT( 1, "xv68k: WARNING: Zero-length code input, exiting" );
	}
	
	if ( path != NULL )
	{
		close( fd );
	}
}

static
v68k::op_result bkpt_2( v68k::processor_state& s )
{
	v68k::op_result result = bridge_call( s );
	
	if ( result >= 0 )
	{
		s.acknowledge_breakpoint( 0x4E75 );  // RTS
	}
	else if ( result == v68k::Illegal_instruction )
	{
		// FIXME:  Report call number
		
		//const uint16_t call_number = emu.d(0);
		
		const char* msg = "Unimplemented system call\n";
		
		write( STDERR_FILENO, msg, strlen( msg ) );
	}
	
	return result;
}

static
v68k::op_result bkpt_3( v68k::processor_state& s )
{
	int32_t new_opcode = v68k::callout::bridge( s );
	
	if ( new_opcode < 0 )
	{
		return v68k::op_result( new_opcode );
	}
	
	if ( new_opcode > 0 )
	{
		s.acknowledge_breakpoint( new_opcode );
	}
	
	return v68k::Ok;
}

static
v68k::op_result bkpt_handler( v68k::processor_state& s, int vector )
{
	switch ( vector )
	{
		case 2:
			return ::bkpt_2( s );
		
		case 3:
			return ::bkpt_3( s );
		
		default:
			return v68k::Illegal_instruction;
	}
}

static inline
unsigned parse_instruction_limit( const char* var )
{
	if ( var == NULL )
	{
		return 0;
	}
	
	return gear::parse_unsigned_decimal( var );
}

static
void emulation_loop( v68k::emulator& emu )
{
	emu.regs[ 8 + 6 ] = 0;
	
	const char* instruction_limit_var = getenv( "XV68K_INSTRUCTION_LIMIT" );
	
	const unsigned max_steps = parse_instruction_limit( instruction_limit_var );
	
	bool event_poll_interrupt_pending = false;
	
	while ( (turbo  &&  native_override( emu ))  ||  emu.step() )
	{
		n_instructions = emu.instruction_count();
		
		if ( max_steps != 0  &&  emu.instruction_count() > max_steps )
		{
			print_instruction_limit_exceeded( instruction_limit_var );
			
			dump_and_raise( emu, SIGXCPU );
		}
		
	#ifdef __RELIX__
		
		if ( short( n_instructions ) == 0 )
		{
			kill( 1, 0 );  // Guaranteed yield point in MacRelix
		}
		
	#endif
		
		using v68k::mac::ticking;
		
		if ( (short( n_instructions ) == 0  ||  ticking)  &&  polling )
		{
			ticking = false;
			
			event_poll_interrupt_pending = true;
		}
		
		if ( event_poll_interrupt_pending  &&  emu.sr.iii == 0 )
		{
			event_poll_interrupt_pending = false;
			
			const int level  = 1;
			const int vector = 64;
			
			emu.interrupt( level, vector );
		}
		
		if ( sigint_pending )
		{
			sigint_pending = false;
			
			const int signal_number =  2;
			const int signal_vector = 64 + signal_number;
			
			emu.interrupt( 7, signal_vector );
		}
	}
}

static
void report_condition( v68k::emulator& emu )
{
	print_blank_line();
	
	switch ( emu.condition )
	{
		using namespace v68k;
		
		case halted:
			print_halted();
			
			dump_and_raise( emu, SIGSEGV );
			break;
		
		case stopped:
			print_stopped();
			break;
		
		default:
			break;
	}
}

void load_file( uint8_t* mem, const char* path )
{
	typedef uint32_t u32;
	
	u32 size;
	
	void* alloc = v68k::utils::load_file( path, &size );
	
	if ( alloc == NULL )
	{
		more::perror( "xv68k", path );
		
		exit( 1 );
	}
	
	if ( size == 0 )
	{
		EXIT( 1, "xv68k: ERROR: Zero-length module file" );
	}
	
	using namespace v68k::alloc;
	
	const int n = (size + page_size - 1) / page_size;  // round up
	
	const u32 addr = allocate_n_pages_for_existing_alloc_unchecked( n, alloc );
	
	if ( addr == 0 )
	{
		more::perror( "xv68k", path, ENOMEM );
		
		exit( 1 );
	}
	
	if ( const char* filename = strrchr( path, '/' ) )
	{
		path = filename + 1;
	}
	
	using v68k::hex32_t;
	
	NOTICE = hex32_t( addr ), " -> ", hex32_t( addr + size ), ": ", path;
	
	uint16_t* p = (uint16_t*) (mem + code_address);
	
	*p++ = iota::big_u16( 0x4EF9 );
	*p++ = iota::big_u16( addr >> 16 );
	*p++ = iota::big_u16( addr );
	*p++ = iota::big_u16( 0x4E75 );  // Terminates function name lookup
}

static
int execute_68k( int argc, char* const* argv )
{
	using v68k::user_data_space;
	
	uint8_t* mem = (uint8_t*) calloc( 1, mem_size );
	
	if ( mem == NULL )
	{
		abort();
	}
	
	const memory_manager memory( mem, mem_size );
	
	v68k::emulator emu( mc68k_model, memory, bkpt_handler );
	
	errno_ptr_addr = params_addr + 2 * sizeof (uint32_t);
	
	atexit( &atexit_report );
	
	v68k::user::os_load_spec load = { mem, mem_size, os_address };
	
	load_vectors( load );
	
	enum
	{
		ScreenRow = 0x0106,
		MemTop    = 0x0108,
		CPUFlag   = 0x012F,
		SoundBase = 0x0266,
		ScrnBase  = 0x0824,
		CrsrPin   = 0x0834,
		CrsrSave  = 0x088C,
	};
	
	emu.put_byte( CPUFlag, mc68k_model >> 4, v68k::user_data_space );
	
	if ( n_512K_blocks >= 0 )
	{
		alt_screen_addr  = mem_size - (0x20000 - 0x12700);
		main_screen_addr = mem_size - (0x20000 - 0x1A700);
		main_sound_addr  = mem_size - (0x20000 - 0x1FD00);
		
		emu.put_long( MemTop,      mem_size,         user_data_space );
	}
	
	using v68k::screen::virtual_buffer;
	
	if ( ! virtual_buffer )
	{
		page_1_virtual_buffer = mem + main_screen_addr;
		page_2_virtual_buffer = mem + alt_screen_addr;
		
		virtual_buffer = page_1_virtual_buffer;
		
		memset( page_1_virtual_buffer, 0xFF, 64 * 342 );
		memset( page_2_virtual_buffer, 0xFF, 64 * 342 );
	}
	
	using v68k::screen::the_surface_shape;
	
	const uint32_t screen_row = the_surface_shape.stride;
	const uint32_t screen_res = the_surface_shape.width
	                          | the_surface_shape.height << 16;
	
	emu.put_word( ScreenRow,   screen_row,       user_data_space );
	emu.put_long( SoundBase,   main_sound_addr,  user_data_space );
	emu.put_long( ScrnBase,    main_screen_addr, user_data_space );
	emu.put_long( CrsrPin + 4, screen_res,       user_data_space );
	
	if ( has_cursor )
	{
		emu.put_long( CrsrSave, kHardwareCursor, user_data_space );
	}
	
	load_Mac_traps( mem );
	
	char* empty_module_argv[] = { NULL };
	
	for ( const module_spec* m = module_specs;  m->name != NULL;  ++m  )
	{
		int module_argc = 0;
		
		char* const* module_argv = empty_module_argv;
		
		if ( m->begin != NULL )
		{
			module_argv = m->begin;
			module_argc = m->end - m->begin;
		}
		
		load_argv( mem, module_argc, module_argv );
		
		load_file( mem, m->name );
		
		emu.reset();
		
		emulation_loop( emu );
		
		if ( emu.condition != v68k::startup )
		{
			more::perror( "xv68k", m->name, "Module installation failed" );
			
			exit( 1 );
		}
	}
	
	load_argv( mem, argc, argv );
	
	const char* path = argv[0];
	
	if ( tracing )
	{
		uint16_t* p = (uint16_t*) (mem + code_address);
		
		*p++ = v68k::big_word( 0x4EB8 );  // JSR
		*p++ = v68k::big_word( 0xFFF4 );  //   trace_on
		
		mem += 4;
	}
	
	load_code( mem, path );
	
	emu.reset();
	
	emulation_loop( emu );
	
	report_condition( emu );
	
	dump( emu );
	
	return 1;
}

static inline
bool begins_array( const char* param )
{
	return param[ 0 ] == '['  &&  param[ 1 ] == '\0';
}

static inline
bool ends_array( const char* param )
{
	return param[ 0 ] == ']'  &&  param[ 1 ] == '\0';
}

static
char* const* get_options( char** argv )
{
	module_spec* module = module_specs;
	
	int opt;
	
	++argv;  // skip arg 0
	
	while ( (opt = command::get_option( (char* const**) &argv, options )) > 0 )
	{
		using command::global_result;
		
		using gear::parse_unsigned_decimal;
		
		switch ( opt )
		{
			case Opt_authorized:
				fully_authorized = true;
				
				if ( fake_pid == 0 )
				{
					fake_pid = -1;
				}
				
				// fall through
			
			case Opt_supervisor:
				supervisor_mode_switch_allowed = true;
				break;
			
			case Opt_ignore_screen_locks:
				ignore_screen_locks();
				break;
			
			case Opt_poll:
				polling = true;
				break;
			
			case Opt_trace:
				tracing = true;
				break;
			
			case Opt_turbo:
				turbo = true;
				break;
			
			case Opt_verbose:
				verbose = true;
				break;
			
			case Opt_mem:
				n_512K_blocks = parse_unsigned_decimal( &global_result.param );
				
				mem_size = n_512K_blocks ? 1024 * 512 * n_512K_blocks
				                         : 1024 * 128;
				break;
			
			case Opt_model:
				{
					const char* p = global_result.param;
					
					if ( *p++ == '6'  &&  *p++ == '8'  &&  *p++ == '0' )
					{
						uint8_t x = *p++ - '0';
						
						if ( x <= 4  &&  *p++ == '0' )
						{
							mc68k_model = v68k::processor_model( x << 4 );
							goto ok;
						}
					}
					
					EXIT( 2, "xv68k: invalid --model parameter" );
				ok:
					;
				}
				break;
			
			case Opt_pid:
				if ( global_result.param )
				{
					fake_pid = parse_unsigned_decimal( &global_result.param );
				}
				else
				{
					fake_pid = -1;
				}
				
				break;
			
			case Opt_raster:
				
			#ifdef __RELIX__
				
				EXIT( 1, "xv68k: --raster support missing" );
				
			#endif
				
				if ( has_screen )
				{
					write( STDERR_FILENO,
					       STR_LEN( "xv68k: duplicate --raster switch\n" ) );
					
					exit( 2 );
				}
				
				const char* path;
				path = global_result.param;
				
				if ( int nok = set_screen_backing_store_file( path ) )
				{
					const char* error = strerror( nok );
					
					write( STDERR_FILENO, path, strlen( path ) );
					write( STDERR_FILENO, STR_LEN( ": " ) );
					write( STDERR_FILENO, error, strlen( error ) );
					write( STDERR_FILENO, STR_LEN( "\n" ) );
					
					exit( 1 );
				}
				
				has_screen = true;
				
				using v68k::screen::the_screen_size;
				using v68k::screen::virtual_buffer;
				
				if ( the_screen_size != 21888 )
				{
					virtual_buffer = malloc( the_screen_size );
					
					memset( virtual_buffer, 0xFF, the_screen_size );
					
					page_1_virtual_buffer = (uint8_t*) virtual_buffer;
					
					main_screen_addr = 0x00E00000;
				}
				
				break;
			
			case Opt_cursor:
				set_cursor_backing_store_file( global_result.param );
				
				has_cursor = true;
				break;
			
			case Opt_module:
				module->name = global_result.param;
				
				if ( begins_array( global_result.param ) )
				{
					module->name = *argv;
					
					module->begin = argv;
					
					while ( true )
					{
						if ( *argv == NULL )
						{
							EXIT( 2, "Unterminated module argument vector" );
						}
						
						if ( ends_array( *argv ) )
						{
							break;
						}
						
						++argv;
					}
					
					if ( module->begin == argv )
					{
						EXIT( 2, "Empty module argument vector" );
					}
					
					*argv = NULL;
					
					module->end = argv++;
				}
				else
				{
					module->begin = NULL;
					module->end   = NULL;
				}
				
				++module;
				
				break;
			
			default:
				break;
		}
	}
	
	module->name = NULL;
	
	return argv;
}

int main( int argc, char** argv )
{
	if ( argc == 0 )
	{
		static const char* new_argv[] = { "", NULL };
		
		argc = 1;
		argv = (char**) new_argv;
	}
	
	struct sigaction action = { 0 };
	action.sa_handler = &sigint_handler;
	
	int nok = sigaction( SIGINT, &action, NULL );
	
	module_specs = (module_spec*) alloca( argc * sizeof (module_spec) );
	
	char* const* args = get_options( argv );
	
	int argn = argc - (args - argv);
	
	return execute_68k( argn, args );
}
