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

// gear
#include "gear/inscribe_decimal.hh"
#include "gear/parse_decimal.hh"

// v68k
#include "v68k/emulator.hh"
#include "v68k/endian.hh"

// v68k-auth
#include "auth/auth.hh"

// v68k-mac
#include "v68k-mac/trap_dispatcher.hh"

// v68k-user
#include "v68k-user/load.hh"

// v68k-callbacks
#include "callback/bridge.hh"

// v68k-utils
#include "utils/load.hh"
#include "utils/print_register_dump.hh"

// v68k-syscalls
#include "syscall/bridge.hh"
#include "syscall/handler.hh"

// xv68k
#include "diagnostics.hh"
#include "memory.hh"
#include "screen.hh"


#pragma exceptions off


#define STRLEN( s )  (sizeof "" s - 1)

#define STR_LEN( s )  "" s, (sizeof s - 1)


using v68k::big_longword;

using v68k::auth::fully_authorized;

static bool verbose;

static unsigned long n_instructions;


static void atexit_report()
{
	if ( verbose )
	{
		const char* count = gear::inscribe_unsigned_decimal( n_instructions );
		
		write( STDERR_FILENO, STR_LEN( "### Instruction count: " ) );
		write( STDERR_FILENO, count, strlen( count ) );
		write( STDERR_FILENO, STR_LEN( "\n" ) );
	}
}

static void dump( const v68k::processor_state& s )
{
	using v68k::utils::print_register_dump;
	
	print_register_dump( s.regs, s.get_SR() );
}

static void dump_and_raise( const v68k::processor_state& s, int signo )
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
		| OS / supervisor stack |  1K
	3K	+-----------------------+
		|                       |
	4K	|                       |
		|                       |
		| Toolbox trap table    |
		|                       |
		|                       |
		|                       |  4K
	7K	+-----------------------+
		| boot code             |  1K
	8K	+-----------------------+
		|                       |
		|                       |
		|                       |
		| argc/argv/envp params |
		|                       |
		|                       |
		|                       |  4K
	12K	+-----------------------+
		|                       |
		|                       |
		|                       |
		| user stack            |
		|                       |
		|                       |
		|                       |  4K
	16K	+-----------------------+
		|                       |
		|                       |
		|                       |
		|                       |
		|                       |
		|                       |
		|                       |
		= user code             =  x6
		|                       |
		|                       |
		|                       |
		|                       |
		|                       |
		|                       |
		|                       |  48K
	64K	+-----------------------+
		
	104	+-----------------------+
		|                       |  screen memory begins 0x0001A700  (1792 bytes after)
		|                       |  screen memory ends   0x0001FC80  (896 bytes before)
		|                       |
		= screen memory buffer  =  x6
		|                       |
		|                       |
		|                       |  24K (21.375K used)
	128	+-----------------------+
	
*/

const uint32_t params_max_size = 4096;
const uint32_t code_max_size   = 48 * 1024;

const uint32_t os_address   = 2048;
const uint32_t boot_address = 7168;
const uint32_t initial_SSP  = 3072;
const uint16_t initial_USP  = 16384;
const uint32_t code_address = 16384;

const uint32_t os_trap_table_address = 1024;
const uint32_t tb_trap_table_address = 3072;

const uint32_t os_trap_count = 1 <<  8;  //  256, 1K
const uint32_t tb_trap_count = 1 << 10;  // 1024, 4K

const uint32_t mem_size = code_address + code_max_size;

const uint32_t params_addr = 8192;

const uint16_t user_pb_addr   = params_addr +  0;  // 20 bytes
const uint16_t system_pb_addr = params_addr + 20;  // 20 bytes

const uint16_t argc_addr = params_addr + 40;  // 4 bytes
const uint16_t argv_addr = params_addr + 44;  // 4 bytes
const uint32_t args_addr = params_addr + 48;


static void init_trap_table( uint32_t* table, uint32_t* end, uint32_t address )
{
	const uint32_t unimplemented = v68k::big_longword( address );
	
	for ( uint32_t* p = table;  p < end;  ++p )
	{
		*p = unimplemented;
	}
}


static const uint16_t loader_code[] =
{
	0x027C,  // ANDI #DFFF,SR  ; clear S
	0xDFFF,
	
	0x4FF8,  // LEA  (3072).W,A7
	initial_USP,
	
	0x4BF8,  // LEA  (12288).W,A5
	0x3000,
	
	0x42B8,  // CLR.L  user_pb_addr + 2 * sizeof (uint32_t)  ; user_pb->errno_var
	user_pb_addr + 2 * sizeof (uint32_t),
	
	0x21FC,  // MOVE.L  #user_pb_addr,(system_pb_addr).W  ; pb->current_user
	0x0000,
	user_pb_addr,
	system_pb_addr,
	
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

#define HANDLER( handler )  handler, sizeof handler

static void load_vectors( v68k::user::os_load_spec& os )
{
	uint32_t* vectors = (uint32_t*) os.mem_base;
	
	memset( vectors, 0xFF, 1024 );
	
	vectors[0] = big_longword( initial_SSP );  // isp
	
	using v68k::user::install_exception_handler;
	using v68k::mac::trap_dispatcher;
	
	install_exception_handler( os, 10, HANDLER( trap_dispatcher ) );
	install_exception_handler( os, 32, HANDLER( system_call ) );
	
	os.mem_used = boot_address;
	
	install_exception_handler( os,  1, HANDLER( loader_code ) );
	
	using namespace v68k::callback;
	
	vectors[ 4] = big_longword( callback_address( illegal_instruction ) );
	vectors[ 5] = big_longword( callback_address( division_by_zero    ) );
	vectors[ 6] = big_longword( callback_address( chk_trap            ) );
	vectors[ 7] = big_longword( callback_address( trapv_trap          ) );
	vectors[ 8] = big_longword( callback_address( privilege_violation ) );
	vectors[ 9] = big_longword( callback_address( trace_exception     ) );
	vectors[11] = big_longword( callback_address( line_F_emulator     ) );
}

static void load_Mac_traps( uint8_t* mem )
{
	uint32_t* os_traps = (uint32_t*) &mem[ os_trap_table_address ];
	uint32_t* tb_traps = (uint32_t*) &mem[ tb_trap_table_address ];
	
	const uint32_t unimplemented = callback_address( v68k::callback::unimplemented_trap );
	
	init_trap_table( os_traps, os_traps + os_trap_count, unimplemented );
	init_trap_table( tb_traps, tb_traps + tb_trap_count, unimplemented );
	
	using namespace v68k::callback;
	
	os_traps[ 0x1E ] = big_longword( callback_address( NewPtr_trap     ) );
	os_traps[ 0x1F ] = big_longword( callback_address( DisposePtr_trap ) );
	os_traps[ 0x2E ] = big_longword( callback_address( BlockMove_trap  ) );
	os_traps[ 0xAD ] = big_longword( callback_address( Gestalt_trap    ) );
	
	const uint32_t big_no_op = big_longword( callback_address( v68k::callback::no_op ) );
	
	os_traps[ 0x46 ] = big_no_op;  // GetTrapAddress
	os_traps[ 0x47 ] = big_no_op;  // SetTrapAddress
	os_traps[ 0x55 ] = big_no_op;  // StripAddress
	os_traps[ 0x98 ] = big_no_op;  // HWPriv
}

static void load_argv( uint8_t* mem, int argc, char** argv )
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
		
		const size_t len = strlen( *argv ) + 1;
		
		if ( len > args_limit - args_data )
		{
			err_argv_too_big();
			
			abort();
		}
		
		memcpy( args_data, *argv, len );
		
		args_data += len;
		
		++argv;
	}
	
	*args = 0;  // trailing NULL of argv
}

static void load_code( uint8_t* mem, const char* path )
{
	int fd;
	
	if ( path == NULL  ||  (path[0] == '-'  &&  path[1] == '\0') )
	{
		fd = STDIN_FILENO;
	}
	else
	{
		using v68k::utils::load_file;
		
		uint32_t size;
		
		if ( void* alloc = load_file( path, &size ) )
		{
			if ( size > code_max_size )
			{
				abort();
			}
			
			memcpy( mem + code_address, alloc, size );
			
			return;
		}
		
		fd = open( path, O_RDONLY );
		
		if ( fd < 0 )
		{
			abort();
		}
	}
	
	ssize_t n_read = read( fd, mem + code_address, code_max_size );
	
	if ( n_read < 0 )
	{
		abort();
	}
	
	if ( path != NULL )
	{
		close( fd );
	}
}

static uint16_t bkpt_2( v68k::processor_state& s )
{
	if ( bridge_call( s ) )
	{
		return 0x4E75;  // RTS
	}
	else
	{
		// FIXME:  Report call number
		
		//const uint16_t call_number = emu.d(0);
		
		const char* msg = "Unimplemented system call\n";
		
		write( STDERR_FILENO, msg, strlen( msg ) );
		
		return 0x4AFC;  // ILLEGAL
	}
}

static uint16_t bkpt_3( v68k::processor_state& s )
{
	if ( uint32_t new_opcode = v68k::callback::bridge( s ) )
	{
		return new_opcode;
	}
	
	return 0x4AFC;  // ILLEGAL
}

static uint16_t bkpt_handler( v68k::processor_state& s, int vector )
{
	switch ( vector )
	{
		case 2:
			return ::bkpt_2( s );
		
		case 3:
			return ::bkpt_3( s );
		
		default:
			return 0x4AFC;  // ILLEGAL
	}
}

static inline unsigned parse_instruction_limit( const char* var )
{
	if ( var == NULL )
	{
		return 0;
	}
	
	return gear::parse_unsigned_decimal( var );
}

static void emulation_loop( v68k::emulator& emu )
{
	const char* instruction_limit_var = getenv( "XV68K_INSTRUCTION_LIMIT" );
	
	const unsigned instruction_limit = parse_instruction_limit( instruction_limit_var );
	
	while ( emu.step() )
	{
		n_instructions = emu.instruction_count();
		
		if ( short( n_instructions ) == 0 )
		{
			kill( 1, 0 );  // Guaranteed yield point in MacRelix
		}
		
		if ( instruction_limit != 0  &&  emu.instruction_count() > instruction_limit )
		{
			print_instruction_limit_exceeded( instruction_limit_var );
			
			dump_and_raise( emu, SIGXCPU );
		}
	}
}

static void report_condition( v68k::emulator& emu )
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

static int bad_usage( const char* text, size_t text_size, const char* arg )
{
	write( STDERR_FILENO, text, text_size );
	write( STDERR_FILENO, arg, strlen( arg ) );
	write( STDERR_FILENO, STR_LEN( "\n" ) );
	
	return 2;
}

#define BAD_USAGE( text, arg )  bad_usage( STR_LEN( text ": " ), arg )

static const char* find_char( const char* begin, char c )
{
	while ( *begin != '\0'  &&  *begin != c )
	{
		++begin;
	}
	
	return begin;
}

static bool option_matches( const char*  option,
                            size_t       option_size,
                            const char*  name,
                            size_t       name_size )
{
	return option_size == name_size  &&  memcmp( option, name, name_size ) == 0;
}

#define OPTION_MATCHES( option, size, name )  option_matches( option, size, STR_LEN( name ) )

static int execute_68k( int argc, char** argv )
{
	if ( argc > 0 )
	{
		// skip argv[0] if present (which it should be, but we have to check)
		--argc;
		++argv;
	}
	
	char** args = argv - 1;
	
	while ( const char* arg = *++args )
	{
		if ( arg[0] == '-' )
		{
			if ( arg[1] == '\0' )
			{
				// An "-" argument is not an option and means /dev/fd/0
				break;
			}
			
			if ( arg[1] == '-' )
			{
				// long option or "--"
				
				const char* option = arg + 2;
				
				if ( *option == '\0' )
				{
					++args;
					break;
				}
				
				const char* equals = find_char( option, '=' );
				
				const size_t size = equals - option;
				
				if ( OPTION_MATCHES( option, size, "pid" ) )
				{
					if ( *equals == '\0' )
					{
						fake_pid = 0;
					}
					else
					{
						const char* param = equals + 1;
						
						fake_pid = gear::parse_unsigned_decimal( &param );
						
						if ( param[0] != '\0' )
						{
							return BAD_USAGE( "Invalid option", arg );
						}
					}
					
					continue;
				}
				
				if ( OPTION_MATCHES( option, size, "screen" ) )
				{
					const char* screen;
					
					if ( *equals == '\0' )
					{
						screen = *++args;
						
						if ( screen == NULL )
						{
							return BAD_USAGE( "Missing argument", option );
						}
					}
					else
					{
						screen = equals + 1;
					}
					
					int nok = set_screen_backing_store_file( screen );
					
					if ( nok )
					{
						const char* error = strerror( nok );
						
						write( STDERR_FILENO, screen, strlen( screen ) );
						write( STDERR_FILENO, STR_LEN( ": " ) );
						write( STDERR_FILENO, error, strlen( error ) );
						write( STDERR_FILENO, STR_LEN( "\n" ) );
						
						return 1;
					}
					
					continue;
				}
				
				if ( OPTION_MATCHES( option, size, "verbose" ) )
				{
					verbose = true;
					
					continue;
				}
				
				return BAD_USAGE( "Unknown option", arg );
			}
			
			// short option
			
			const char* opt = arg + 1;
			
			if ( opt[0] == 'A' )
			{
				fully_authorized = true;
				
				fake_pid = 0;
				
				continue;
			}
			
			if ( opt[0] == 'v' )
			{
				verbose = true;
				
				continue;
			}
			
			return BAD_USAGE( "Unknown option", arg );
		}
		
		// not an option
		break;
	}
	
	argc -= args - argv;
	argv  = args;
	
	uint8_t* mem = (uint8_t*) calloc( 1, mem_size );
	
	if ( mem == NULL )
	{
		abort();
	}
	
	v68k::user::os_load_spec load = { mem, mem_size, os_address };
	
	load_vectors( load );
	
	load_Mac_traps( mem );
	
	load_argv( mem, argc, argv );
	
	const char* path = argv[0];
	
	load_code( mem, path );
	
	errno_ptr_addr = params_addr + 2 * sizeof (uint32_t);
	
	const memory_manager memory( mem, mem_size );
	
	v68k::emulator emu( v68k::mc68000, memory, bkpt_handler );
	
	emu.reset();
	
	atexit( &atexit_report );
	
	emulation_loop( emu );
	
	report_condition( emu );
	
	dump( emu );
	
	return 1;
}

int main( int argc, char** argv )
{
	return execute_68k( argc, argv );
}

