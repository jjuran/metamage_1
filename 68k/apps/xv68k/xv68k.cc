/*
	xv68k.cc
	--------
*/

// Standard C
#include <signal.h>
#include <stdlib.h>
#include <string.h>

// POSIX
#include <unistd.h>

// gear
#include "gear/inscribe_decimal.hh"
#include "gear/parse_decimal.hh"

// log-of-war
#include "logofwar/report.hh"

// command
#include "command/get_option.hh"

// v68k
#include "v68k/emulator.hh"

// v68k-auth
#include "auth/auth.hh"

// v68k-ioutils
#include "ioutils/print_register_dump.hh"

// v68k-callouts
#include "callout/bridge.hh"
#include "callout/cursor_file.hh"

// v68k-screen
#include "screen/lock.hh"
#include "screen/screen.hh"

// v68k-sound
#include "sound/sound.hh"

// v68k-syscalls
#include "syscall/bridge.hh"

// v68k-memory
#include "memory/buffers.hh"
#include "memory/memory.hh"
#include "memory/screen.hh"

// v68k-diagnostics
#include "diagnostics/diagnostics.hh"

// v68k-exec
#include "exec/load.hh"
#include "exec/loop.hh"

// v68k-os
#include "os/load.hh"


#ifdef __MWERKS__
#pragma exceptions off
#endif


#define STR_LEN( s )  "" s, (sizeof s - 1)


using namespace v68k::diagnostics;

using logofwar::hex32_t;

using v68k::auth::fully_authorized;
using v68k::auth::supervisor_mode_switch_allowed;
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

using v68k::exec::turbo;
using v68k::exec::polling;
using v68k::exec::tracing;

static bool verbose;
static bool has_screen;
static bool has_cursor;

using v68k::exec::n_instructions;

static int n_512K_blocks = -1;

using v68k::exec::module_spec;
using v68k::exec::module_specs;


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
	
	Opt_xv,   // exception vector access
	Opt_mem,  // number of 512K memory chunks, or zero for 128K
	Opt_pid,
	Opt_raster,
	Opt_cursor,
	Opt_snd_fd,
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
	{ "xv",         Opt_xv,     command::Param_required },
	{ "mem",        Opt_mem,    command::Param_required },
	{ "model",      Opt_model,  command::Param_required },
	{ "pid",        Opt_pid,    command::Param_optional },
	{ "raster",     Opt_raster, command::Param_required },
	{ "cursor",     Opt_cursor, command::Param_required },
	{ "module",     Opt_module, command::Param_required },
	{ "sound-fd",   Opt_snd_fd, command::Param_required },
	
	{ "ignore-screen-locks", Opt_ignore_screen_locks },
	
	{ NULL }
};


using v68k::exec::sigint_pending;

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
	using v68k::ioutils::print_register_dump;
	
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

using v68k::exec::params_addr;

static uint32_t mem_size = 1024 * 128;


static
int32_t bkpt_2( v68k::processor_state& s )
{
	v68k::op_result result = bridge_call( s );
	
	if ( result >= 0 )
	{
		return 0x4E75;  // RTS
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

using v68k::exec::emulation_loop;

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

static
int execute_68k( int argc, char* const* argv )
{
	using v68k::user_data_space;
	
	uint8_t* mem = (uint8_t*) calloc( 1, mem_size );
	
	if ( mem == NULL )
	{
		abort();
	}
	
	const v68k::memory::memory_manager memory( mem, mem_size );
	
	v68k::emulator emu( mc68k_model, memory );
	
	emu.bkpt[ 2 ] = &bkpt_2;
	emu.bkpt[ 3 ] = &v68k::callout::bridge;
	
	errno_ptr_addr = params_addr + 2 * sizeof (uint32_t);
	
	atexit( &atexit_report );
	
	v68k::os::load( mem, mem_size );
	
	const uint32_t mem_top = n_512K_blocks >= 0 ? mem_size : 0;
	
	v68k::memory::place_buffers( emu, mem, mem_top );
	
	enum
	{
		CPUFlag   = 0x012F,
		CrsrSave  = 0x088C,
	};
	
	emu.put_byte( CPUFlag, mc68k_model >> 4, v68k::user_data_space );
	
	if ( has_cursor )
	{
		emu.put_long( CrsrSave, kHardwareCursor, user_data_space );
	}
	
	using v68k::exec::load_and_run;
	
	load_and_run( mem, emu, argc, argv );
	
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
			
			case Opt_xv:
				int xv;
				
				xv = (strcmp( global_result.param, "real" ) == 0)
				   - (strcmp( global_result.param, "fake" ) == 0);
				
				if ( ! xv  &&  strcmp( global_result.param, "none" ) != 0 )
				{
					EXIT( 2, "xv68k: invalid --xv parameter" );
				}
				
				using v68k::memory::major_system_vector_access;
				using v68k::memory::usermode_memory_access;
				
				major_system_vector_access = usermode_memory_access( xv );
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
				
				using v68k::screen::set_screen_backing_store_file;
				using v68k::memory::allocate_screen;
				
				const char* path;
				path = global_result.param;
				
				int nok;
				
				nok = set_screen_backing_store_file( path )  ||
				      allocate_screen();
				
				if ( nok )
				{
					const char* error = strerror( nok );
					
					write( STDERR_FILENO, path, strlen( path ) );
					write( STDERR_FILENO, STR_LEN( ": " ) );
					write( STDERR_FILENO, error, strlen( error ) );
					write( STDERR_FILENO, STR_LEN( "\n" ) );
					
					exit( 1 );
				}
				
				has_screen = true;
				
				break;
			
			case Opt_cursor:
				using v68k::callout::set_cursor_backing_store_file;
				
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
			
			case Opt_snd_fd:
				using v68k::sound::sound_fd;
				
				sound_fd = parse_unsigned_decimal( global_result.param );
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
