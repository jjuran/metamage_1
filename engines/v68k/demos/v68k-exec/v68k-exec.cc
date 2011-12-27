/*
	v68k-hello.cc
	-------------
*/

// Standard C
#include <signal.h>
#include <stdlib.h>
#include <string.h>

// POSIX
#include <fcntl.h>
#include <unistd.h>

// v68k
#include "v68k/endian.hh"

// v68k-user
#include "v68k-user/line_A_shim.hh"
#include "v68k-user/load.hh"

// v68k-callbacks
#include "callback/bridge.hh"

// v68k-syscalls
#include "syscall/bridge.hh"
#include "syscall/handler.hh"

// v68k-exec
#include "memory.hh"


#pragma exceptions off


using v68k::big_longword;


/*
	Memory map
	----------
	
	0K	+-----------------------+
		| System vectors        |
	1K	+-----------------------+
		| OS / supervisor stack |
	2K	+-----------------------+
		|                       |
		|                       |
		|                       |
	4K	+-----------------------+
		|                       |
		|                       |
		|                       |
		| argc/argv/envp params |
		|                       |
		|                       |
		|                       |
	8K	+-----------------------+
		|                       |
		|                       |
		|                       |
		| user stack            |
		|                       |
		|                       |
		|                       |
	12K	+-----------------------+
		|                       |
		|                       |
		|                       |
		|                       |
		|                       |
		|                       |
		|                       |
		= user code             =  x4
		|                       |
		|                       |
		|                       |
		|                       |
		|                       |
		|                       |
		|                       |
	44K	+-----------------------+
	
*/

const uint32_t params_max_size = 4096;
const uint32_t code_max_size   = 32768;

const uint32_t os_address   = 1024;
const uint32_t initial_SSP  = 2048;
const uint32_t initial_USP  = 12288;
const uint32_t code_address = 12288;

const uint32_t mem_size = code_address + code_max_size;

const uint32_t params_addr = 4096;

const uint32_t user_pb_addr   = params_addr +  0;  // 20 bytes
const uint32_t system_pb_addr = params_addr + 20;  // 20 bytes

const uint32_t argc_addr = params_addr + 40;  // 4 bytes
const uint32_t argv_addr = params_addr + 44;  // 4 bytes
const uint32_t args_addr = params_addr + 48;


static const uint16_t bkpt_7_code[] =
{
	// Illegal Instruction,
	// Privilege Violation
	
	0x484F  // BKPT  #7
};

static const uint16_t loader_code[] =
{
	0x027C,  // ANDI #DFFF,SR  ; clear S
	0xDFFF,
	
	0x4FF8,  // LEA  (3072).W,A7
	initial_USP,
	
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
	0x7001,  // MOVEQ  #1,D0
	0x484A   // BKPT   #2
};

#define HANDLER( handler )  handler, sizeof handler

static void load_vectors( v68k::user::os_load_spec& os )
{
	uint32_t* vectors = (uint32_t*) os.mem_base;
	
	memset( vectors, 0xFF, 1024 );
	
	vectors[0] = big_longword( initial_SSP );  // isp
	
	using v68k::user::install_exception_handler;
	using v68k::user::line_A_shim;
	
	install_exception_handler( os,  1, HANDLER( loader_code ) );
	install_exception_handler( os,  4, HANDLER( bkpt_7_code ) );
	install_exception_handler( os, 10, HANDLER( line_A_shim ) );
	install_exception_handler( os, 32, HANDLER( system_call ) );
	
	vectors[8] = vectors[4];
}

static int execute_68k( int argc, char** argv )
{
	const char* path = argv[1];
	
	const char* instruction_limit_var = getenv( "V68K_INSTRUCTION_LIMIT" );
	
	const int instruction_limit = instruction_limit_var ? atoi( instruction_limit_var ) : 0;
	
	errno_ptr_addr = params_addr + 2 * sizeof (uint32_t);
	
	uint8_t* mem = (uint8_t*) calloc( 1, mem_size );
	
	if ( mem == NULL )
	{
		abort();
	}
	
	v68k::user::os_load_spec load = { mem, mem_size, os_address };
	
	load_vectors( load );
	
	(uint32_t&) mem[ argc_addr ] = big_longword( argc - 1 );
	(uint32_t&) mem[ argv_addr ] = big_longword( args_addr );
	
	uint32_t* args = (uint32_t*) &mem[ args_addr ];
	
	uint8_t* args_limit = &mem[ params_addr ] + params_max_size;
	
	uint8_t* args_data = (uint8_t*) (args + argc);
	
	if ( args_data >= args_limit )
	{
		abort();
	}
	
	while ( *++argv != NULL )
	{
		*args++ = big_longword( args_data - mem );
		
		const size_t len = strlen( *argv ) + 1;
		
		if ( len > args_limit - args_data )
		{
			abort();
		}
		
		memcpy( args_data, *argv, len );
		
		args_data += len;
	}
	
	*args = 0;  // trailing NULL of argv
	
	if ( path != NULL )
	{
		int fd = open( path, O_RDONLY );
		
		if ( fd >= 0 )
		{
			int n_read = read( fd, mem + code_address, code_max_size );
			
			close( fd );
		}
	}
	
	const memory_manager memory( mem, mem_size );
	
	v68k::emulator emu( v68k::mc68000, memory );
	
	emu.reset();
	
step_loop:
	
	while ( emu.step() )
	{
		if ( instruction_limit != 0  &&  emu.instruction_count() > instruction_limit )
		{
			raise( SIGXCPU );
		}
		
		continue;
	}
	
	if ( emu.condition == v68k::bkpt_2 )
	{
		if ( bridge_call( emu ) )
		{
			emu.acknowledge_breakpoint( 0x4E75 );  // RTS
		}
		
		goto step_loop;
	}
	
	if ( emu.condition == v68k::bkpt_3 )
	{
		if ( uint32_t new_opcode = v68k::callback::bridge( emu ) )
		{
			emu.acknowledge_breakpoint( new_opcode );
		}
		
		goto step_loop;
	}
	
	switch ( emu.condition )
	{
		using namespace v68k;
		
		case halted:
			raise( SIGSEGV );
			break;
		
		case bkpt_0:
		case bkpt_1:
		case bkpt_2:
		case bkpt_3:
		case bkpt_4:
		case bkpt_5:
		case bkpt_6:
		case bkpt_7:
			raise( SIGILL );
			break;
		
		default:
			break;
	}
	
	return 1;
}

int main( int argc, char** argv )
{
	if ( const char* path = argv[1] )
	{
		return execute_68k( argc, argv );
	}
	
	return 0;
}

