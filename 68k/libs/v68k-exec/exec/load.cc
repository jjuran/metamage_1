/*
	load.cc
	-------
*/

#include "exec/load.hh"

// POSIX
#include <unistd.h>

// Standard C
#include <stdlib.h>
#include <string.h>

// more-libc
#include "more/string.h"

// more-posix
#include "more/perror.hh"

// log-of-war
#include "logofwar/report.hh"

// v68k
#include "v68k/endian.hh"

// v68k-alloc
#include "v68k-alloc/memory.hh"

// v68k-ioutils
#include "ioutils/load.hh"
//#include "ioutils/print_register_dump.hh"

// v68k-callouts
#include "callout/bridge.hh"

// v68k-diagnostics
#include "diagnostics/diagnostics.hh"

// v68k-exec
#include "exec/loop.hh"


#ifdef __MWERKS__
#pragma exceptions off
#endif


#define STR_LEN( s )  "" s, (sizeof s - 1)

#define EXIT( status, msg )  exit_with_message( STR_LEN( msg "\n" ), status )

static
void exit_with_message( const char* msg, unsigned len, int status )
{
	write( STDERR_FILENO, msg, len );
	
	exit( status );
}


namespace v68k {
namespace exec {

using namespace v68k::diagnostics;

using v68k::big_longword;

module_spec* module_specs;

bool tracing;

const uint32_t params_max_size = 2048;
const uint32_t code_max_size   = 40 * 1024;

const uint32_t os_trap_table_address = 1024;
const uint32_t tb_trap_table_address = 3072;

const uint32_t os_trap_count = 1 <<  8;  //  256, 1K
const uint32_t tb_trap_count = 1 << 10;  // 1024, 4K

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
void load_Mac_traps( uint8_t* mem )
{
	using namespace v68k::callout;
	
	using v68k::callout::alloc;
	
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
		
		const size_t len = strlen( *argv ) + 1;
		
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
void load_file( uint8_t* mem, const char* path )
{
	typedef uint32_t u32;
	
	u32 size;
	
	void* alloc = v68k::ioutils::load_file( path, &size );
	
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
	
	using logofwar::hex32_t;
	
	if ( const char* filename = strrchr( path, '/' ) )
	{
		path = filename + 1;
	}
	
	NOTICE = hex32_t( addr ), " -> ", hex32_t( addr + size ), ": ", path;
	
	uint16_t* p = (uint16_t*) (mem + code_address);
	
	*p++ = iota::big_u16( 0x4EF9 );
	*p++ = iota::big_u16( addr >> 16 );
	*p++ = iota::big_u16( addr );
	*p++ = iota::big_u16( 0x4E75 );  // Terminates function name lookup
}

static inline
bool is_hyphen( const char* s )
{
	return s[ 0 ] == '-'  &&  s[ 1 ] == '\0';
}

static inline
bool is_real_path( const char* path )
{
	return path  &&  ! is_hyphen( path );
}

static
void load_code( uint8_t* mem, const char* path )
{
	if ( is_real_path( path ) )
	{
		load_file( mem, path );
		
		return;
	}
	
	int fd = STDIN_FILENO;
	
	ssize_t n_read = read( fd, mem + code_address, code_max_size );
	
	if ( n_read < 0 )
	{
		abort();
	}
	
	if ( n_read == 0 )
	{
		EXIT( 1, "xv68k: WARNING: Zero-length code input, exiting" );
	}
}

void load_and_run( uint8_t* mem, emulator& emu, int argc, char* const* argv )
{
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
	
	emulation_loop( emu );
}

}  // namespace exec
}  // namespace v68k
