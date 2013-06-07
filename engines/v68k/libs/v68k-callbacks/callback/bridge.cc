/*
	bridge.cc
	---------
*/

#include "callback/bridge.hh"

// POSIX
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

// Standard C
#include <errno.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>

// must
#include "must/write.h"

// v68k-alloc
#include "v68k-alloc/memory.hh"

// v68k-auth
#include "auth/auth.hh"

// v68k-utils
#include "utils/load.hh"
#include "utils/print_register_dump.hh"


#pragma exceptions off


#define ERR_MSG( msg )  "v68k: exception: " msg "\n"

#define STRLEN( s )  (sizeof "" s - 1)

#define STR_LEN( s )  "" s, (sizeof s - 1)

#define WRITE_ERR( msg )  must_write( STDERR_FILENO, STR_LEN( ERR_MSG( msg ) ) )


namespace v68k     {
namespace callback {

using v68k::auth::fully_authorized;


enum
{
	nop = 0x4E71,
	rts = 0x4E75,
	nil = 0
};

typedef uint32_t (*function_type)( v68k::processor_state& s );


static void dump_and_raise( const v68k::processor_state& s, int signo )
{
	using v68k::utils::print_register_dump;
	
	print_register_dump( s.regs );
	
	raise( signo );
}

static uint32_t unimplemented_callback( v68k::processor_state& s )
{
	abort();
	
	// Not reached
	return nil;
}

static uint32_t no_op_callback( v68k::processor_state& s )
{
	return rts;
}

static uint32_t load_callback( v68k::processor_state& s )
{
	const uint32_t path_addr = s.regs.a[0];
	const uint32_t path_size = s.regs.d[0];  // includes trailing NUL
	
	s.regs.a[0] = 0;
	
	if ( !fully_authorized )
	{
		s.regs.d[1] = EPERM;
		
		return rts;
	}
	
	const uint8_t* p = s.mem.translate( path_addr,
	                                    path_size,
	                                    v68k::user_data_space,
	                                    v68k::mem_read );
	
	if ( p == NULL )
	{
		s.regs.d[1] = EFAULT;
		
		return rts;
	}
	
	if ( path_size == 0  ||  p[0] == '\0'  ||  p[ path_size - 1 ] != '\0' )
	{
		s.regs.d[1] = EINVAL;
		
		return rts;
	}
	
	using v68k::alloc::page_size;
	using v68k::alloc::page_size_bits;
	using v68k::alloc::allocate_n_pages;
	using v68k::utils::load_file;
	
	const char* path = (const char*) p;
	
	void* alloc = load_file( path, &s.regs.d[0] );
	
	const size_t n = (s.regs.d[0] + page_size - 1) >> page_size_bits;  // round up
	
	const uint32_t addr = allocate_n_pages( alloc, n );
	
	if ( addr == 0 )
	{
		free( alloc );
		
		s.regs.d[1] = ENOMEM;
	}
	else
	{
		s.regs.a[0] = addr;
	}
	
	return rts;
}

static uint32_t enter_supervisor_mode_callback( v68k::processor_state& s )
{
	const uint16_t old_SR = s.get_SR();
	const uint16_t new_SR = old_SR | 0x2000;
	
	s.regs.d[0] = old_SR;
	
	if ( old_SR == new_SR )
	{
		return rts;
	}
	
	if ( fully_authorized )
	{
		uint32_t return_address;
		
		if ( !s.mem.get_long( s.regs.a[7], return_address, s.data_space() ) )
		{
			abort();  // FIXME
			return nil;
		}
		
		s.regs.a[7] += 4;
		
		s.set_SR( new_SR );
		
		s.regs.pc = return_address - 2;
		
		return nop;
	}
	else
	{
		s.regs.d[0] = 0xFFFFFFFF;
		s.regs.d[1] = EPERM;
	}
	
	return rts;
}

static uint32_t set_trace_mode_callback( v68k::processor_state& s )
{
	s.regs.ttsm = (s.regs.ttsm & 0x3) | (uint8_t( s.regs.pc ) << 1 & 0xC);
	
	return rts;
}

static char hex[] =
{
	'0', '1', '2', '3',
	'4', '5', '6', '7',
	'8', '9', 'A', 'B',
	'C', 'D', 'E', 'F'
};

#define UNIMPLEMENTED_TRAP_PREFIX  "v68k: exception: Unimplemented Mac trap: "

static uint32_t unimplemented_trap_callback( v68k::processor_state& s )
{
	static char buffer[] = UNIMPLEMENTED_TRAP_PREFIX "A123\n";
	
	char* p = buffer + STRLEN( UNIMPLEMENTED_TRAP_PREFIX );
	
	const uint16_t trap = s.regs.d[1];
	
	p[1] = hex[ trap >> 8 & 0xF ];
	p[2] = hex[ trap >> 4 & 0xF ];
	p[3] = hex[ trap      & 0xF ];
	
	must_write( STDERR_FILENO, buffer, STRLEN( UNIMPLEMENTED_TRAP_PREFIX "A123\n" ) );
	
	dump_and_raise( s, SIGILL );
	
	// Not reached
	return nil;
}

static uint32_t NewPtr_callback( v68k::processor_state& s )
{
	const uint32_t size = s.regs.d[0];
	
	uint32_t addr = v68k::alloc::allocate( size );
	
	s.regs.a[0] = addr;
	
	if ( addr == 0 )
	{
		const uint32_t addr_MemErr = 0x0220;
		
		const int16_t memFullErr = -108;
		
		s.mem.put_word( addr_MemErr, memFullErr, v68k::user_data_space );
	}
	
	return rts;
}

static uint32_t DisposePtr_callback( v68k::processor_state& s )
{
	const uint32_t addr = s.regs.a[0];
	
	v68k::alloc::deallocate( addr );
	
	return rts;
}

static uint32_t BlockMove_callback( v68k::processor_state& s )
{
	const uint32_t src = s.regs.a[0];
	const uint32_t dst = s.regs.a[1];
	
	const uint32_t n = s.regs.d[0];
	
	const uint8_t* p = s.mem.translate( src, n, v68k::user_data_space, v68k::mem_read );
	
	if ( p == NULL )
	{
		abort();
		return nil;  // FIXME
	}
	
	uint8_t* q = s.mem.translate( dst, n, v68k::user_data_space, v68k::mem_write );
	
	if ( q == NULL )
	{
		abort();
		return nil;  // FIXME
	}
	
	memcpy( q, p, n );
	
	s.mem.translate( dst, n, v68k::user_data_space, v68k::mem_update );
	
	return rts;
}

#define OSTYPE(a, b, c, d)  ((a) << 24 | (b) << 16 | (c) << 8 | (d))

static uint32_t Gestalt_callback( v68k::processor_state& s )
{
	const int32_t gestaltUndefSelectorErr = -5551;
	
	const uint32_t selector = s.regs.d[0];
	
	uint32_t value = 0;
	int32_t result = 0;
	
	switch ( selector )
	{
		case OSTYPE('v', '6', '8', 'k'):  // 'v68k'
		//	value = 0;
			break;
		
		case OSTYPE('p', 'r', 'o', 'c'):  // 'proc'
			value = (s.model >> 8) + 1;
			break;
		
		default:
			result = gestaltUndefSelectorErr;
			break;
	}
	
	s.regs.d[0] = result;
	s.regs.a[0] = value;
	
	return rts;
}


static uint32_t illegal_instruction_callback( v68k::processor_state& s )
{
	WRITE_ERR( "Illegal Instruction" );
	
	dump_and_raise( s, SIGILL );
	
	return nil;
}

static uint32_t division_by_zero_callback( v68k::processor_state& s )
{
	WRITE_ERR( "Division By Zero" );
	
	dump_and_raise( s, SIGFPE );
	
	return nil;
}

static uint32_t chk_trap_callback( v68k::processor_state& s )
{
	WRITE_ERR( "CHK range exceeded" );
	
	dump_and_raise( s, SIGFPE );
	
	return nil;
}

static uint32_t trapv_trap_callback( v68k::processor_state& s )
{
	WRITE_ERR( "TRAPV on overflow" );
	
	dump_and_raise( s, SIGFPE );
	
	return nil;
}

static uint32_t privilege_violation_callback( v68k::processor_state& s )
{
	WRITE_ERR( "Privilege Violation" );
	
	dump_and_raise( s, SIGILL );
	
	return nil;
}

static uint32_t trace_exception_callback( v68k::processor_state& s )
{
	WRITE_ERR( "Trace Exception" );
	
	dump_and_raise( s, SIGTRAP );
	
	return nil;
}

static uint32_t line_A_emulator_callback( v68k::processor_state& s )
{
	WRITE_ERR( "Line A Emulator" );
	
	dump_and_raise( s, SIGILL );
	
	return nil;
}

static uint32_t line_F_emulator_callback( v68k::processor_state& s )
{
	WRITE_ERR( "Line F Emulator" );
	
	dump_and_raise( s, SIGILL );
	
	return nil;
}


static const function_type the_callbacks[] =
{
	&no_op_callback,
	&load_callback,
	&enter_supervisor_mode_callback,
	NULL,
	&set_trace_mode_callback,
	&set_trace_mode_callback,
	&set_trace_mode_callback,
	&set_trace_mode_callback,
	&illegal_instruction_callback,
	&division_by_zero_callback,
	&chk_trap_callback,
	&trapv_trap_callback,
	&privilege_violation_callback,
	&trace_exception_callback,
	&line_A_emulator_callback,
	&line_F_emulator_callback,
	&unimplemented_trap_callback,
	&NewPtr_callback,
	&DisposePtr_callback,
	&BlockMove_callback,
	&Gestalt_callback,
	&unimplemented_callback
};


uint32_t bridge( v68k::processor_state& s )
{
	const int32_t pc = s.regs.pc;
	
	const uint32_t call_number = pc / -2 - 1;
	
	const size_t n_callbacks = sizeof the_callbacks / sizeof the_callbacks[0];
	
	if ( call_number < n_callbacks )
	{
		const function_type f = the_callbacks[ call_number ];
		
		if ( f != NULL )
		{
			return f( s );
		}
	}
	
	return nil;
}

}  // namespace callback
}  // namespace v68k

