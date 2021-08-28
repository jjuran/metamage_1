/*
	bridge.cc
	---------
*/

#include "callout/bridge.hh"

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

// log-of-war
#include "logofwar/report.hh"

// v68k
#include "v68k/endian.hh"

// v68k-alloc
#include "v68k-alloc/memory.hh"

// v68k-auth
#include "auth/auth.hh"

// v68k-screen
#include "screen/lock.hh"
#include "screen/surface.hh"
#include "screen/update.hh"

// v68k-syscalls
#include "syscall/bridge.hh"

// v68k-time
#include "v68k-time/clock.hh"

// v68k-utils
#include "utils/load.hh"
#include "utils/print_register_dump.hh"


#ifdef __MWERKS__
#pragma exceptions off
#endif


#define ERR_MSG( msg )  "v68k: exception: " msg "\n"

#define STRLEN( s )  (sizeof "" s - 1)

#define STR_LEN( s )  "" s, (sizeof s - 1)

#define WRITE_ERR( msg )  must_write( STDERR_FILENO, STR_LEN( ERR_MSG( msg ) ) )


namespace v68k    {
namespace callout {

using v68k::mem_read;
using v68k::mem_write;
using v68k::mem_update;

using v68k::auth::fully_authorized;
using v68k::auth::supervisor_mode_switch_allowed;
using v68k::screen::lock_level;


enum
{
	nop = 0x4E71,
	rts = 0x4E75,
	nil = 0
};

typedef int32_t (*function_type)( v68k::processor_state& s );

enum code32_t
{
	code32_min = 0,
	code32_max = 0xffffffff,
};

static
void print( code32_t x )
{
	char buffer[ 7 ];
	
	char* p = buffer;
	
	*p++ = '\'';
	*p++ = x >> 24;
	*p++ = x >> 16;
	*p++ = x >>  8;
	*p++ = x >>  0;
	*p++ = '\'';
	*p++ = '\0';
	
	logofwar::print( buffer );
}

static void dump_and_raise( const v68k::processor_state& s, int signo )
{
	using v68k::utils::print_register_dump;
	
	print_register_dump( s.regs, s.get_SR() );
	
	raise( signo );
}

static
int32_t unimplemented_callout( v68k::processor_state& s )
{
	abort();
	
	// Not reached
	return nil;
}

static
int32_t no_op_callout( v68k::processor_state& s )
{
	return rts;
}

static
int32_t load_callout( v68k::processor_state& s )
{
	const v68k::function_code_t data_space = s.data_space();
	
	const uint32_t path_addr = s.a(0);
	const uint32_t path_size = s.d(0);  // includes trailing NUL
	
	s.a(0) = 0;
	
	if ( !fully_authorized )
	{
		s.d(1) = EPERM;
		
		return rts;
	}
	
	const uint8_t* p = s.mem.translate( path_addr,
	                                    path_size,
	                                    data_space,
	                                    mem_read );
	
	if ( p == NULL )
	{
		s.d(1) = EFAULT;
		
		return rts;
	}
	
	if ( path_size == 0  ||  p[0] == '\0'  ||  p[ path_size - 1 ] != '\0' )
	{
		s.d(1) = EINVAL;
		
		return rts;
	}
	
	using v68k::alloc::page_size;
	using v68k::alloc::page_size_bits;
	using v68k::alloc::allocate_n_pages;
	using v68k::utils::load_file;
	
	const char* path = (const char*) p;
	
	void* alloc = load_file( path, &s.d(0) );
	
	const size_t n = (s.d(0) + page_size - 1) >> page_size_bits;  // round up
	
	const uint32_t addr = allocate_n_pages( alloc, n );
	
	if ( addr == 0 )
	{
		free( alloc );
		
		s.d(1) = ENOMEM;
	}
	else
	{
		s.a(0) = addr;
	}
	
	return rts;
}

static
int32_t enter_supervisor_mode_callout( v68k::processor_state& s )
{
	const uint16_t old_SR = s.get_SR();
	const uint16_t new_SR = old_SR | 0x2000;
	
	s.d(0) = old_SR;
	
	if ( old_SR == new_SR )
	{
		s.sr.nzvc = 4;  // Clear N, set Z
		
		return rts;
	}
	
	if ( supervisor_mode_switch_allowed )
	{
		uint32_t return_address;
		
		if ( !s.mem.get_long( s.a(7), return_address, s.data_space() ) )
		{
			abort();  // FIXME
			return nil;
		}
		
		s.a(7) += 4;
		
		s.set_SR( new_SR );
		
		s.sr.nzvc = 0;  // Clear N, Z
		
		s.pc() = return_address - 2;
		
		return nop;
	}
	else
	{
		s.d(0) = 0xFFFFFFFF;
		s.d(1) = EPERM;
		
		s.sr.nzvc = 8;  // Set N
	}
	
	return rts;
}

static
int32_t module_suspend_callout( v68k::processor_state& s )
{
	s.condition = startup;
	
	return rts;
}

static
int32_t set_trace_mode_callout( v68k::processor_state& s )
{
	s.sr.ttsm = (s.sr.ttsm & 0x3) | (uint8_t( s.pc() ) << 1 & 0xC);
	
	return rts;
}

static
int32_t ScrnBitMap_callout( v68k::processor_state& s )
{
	const v68k::function_code_t data_space = s.data_space();
	
	uint32_t& sp = s.a(7);
	
	uint32_t return_address;
	uint32_t pointer;
	
	if ( ! s.mem.get_long( sp, return_address, data_space ) )
	{
		return v68k::Bus_error;
	}
	
	sp += 4;
	
	if ( ! s.mem.get_long( sp, pointer, data_space ) )
	{
		return v68k::Bus_error;
	}
	
	sp += 4;
	
	const size_t n = sizeof (uint32_t)
	               + sizeof (uint16_t)
	               + sizeof (uint16_t) * 4;
	
	uint8_t* p = s.mem.translate( pointer, n, data_space, mem_write );
	
	if ( p == NULL )
	{
		return v68k::Bus_error;
	}
	
	using v68k::screen::the_surface_shape;
	
	const uint32_t baseAddr = 0x0001A700;
	const uint16_t rowBytes = the_surface_shape.stride;
	const uint16_t height   = the_surface_shape.height;
	const uint16_t width    = the_surface_shape.width;
	
	*p++ = uint8_t( baseAddr >> 24 );
	*p++ = uint8_t( baseAddr >> 16 );
	*p++ = uint8_t( baseAddr >>  8 );
	*p++ = uint8_t( baseAddr       );
	
	*p++ = uint8_t( rowBytes >> 8 );
	*p++ = uint8_t( rowBytes      );
	
	*p++ = 0;
	*p++ = 0;
	
	*p++ = 0;
	*p++ = 0;
	
	*p++ = uint8_t( height >> 8 );
	*p++ = uint8_t( height      );
	
	*p++ = uint8_t( width >> 8 );
	*p++ = uint8_t( width      );
	
	s.pc() = return_address - 2;
	
	return nop;
}

static
int32_t lock_screen_callout( v68k::processor_state& s )
{
	--lock_level;
	
	return rts;
}

static
int32_t unlock_screen_callout( v68k::processor_state& s )
{
	if ( ++lock_level == 0 )
	{
		v68k::screen::update();
	}
	
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

static
int32_t unimplemented_trap_callout( v68k::processor_state& s )
{
	static char buffer[] = UNIMPLEMENTED_TRAP_PREFIX "A123\n";
	
	char* p = buffer + STRLEN( UNIMPLEMENTED_TRAP_PREFIX );
	
	const uint16_t trap = s.d(1);
	
	p[1] = hex[ trap >> 8 & 0xF ];
	p[2] = hex[ trap >> 4 & 0xF ];
	p[3] = hex[ trap      & 0xF ];
	
	must_write( STDERR_FILENO, buffer, STRLEN( UNIMPLEMENTED_TRAP_PREFIX "A123\n" ) );
	
	dump_and_raise( s, SIGILL );
	
	// Not reached
	return nil;
}

enum
{
	noErr      = 0,
	memFullErr = -108,
};

static
int32_t alloc_callout( v68k::processor_state& s )
{
	int32_t err = noErr;
	
	const uint32_t size = s.d(0);
	
	uint32_t addr = v68k::alloc::allocate( size );
	
	s.a(0) = addr;
	
	if ( addr == 0 )
	{
		err = memFullErr;
		
		WARNING = "Memory exhaustion: ", size, "-byte allocation failed";
	}
	
	s.d(0) = err;
	
	return rts;
}

static
int32_t dealloc_callout( v68k::processor_state& s )
{
	const uint32_t addr = s.a(0);
	
	v68k::alloc::deallocate( addr );
	
	s.d(0) = noErr;
	
	return rts;
}

static
int32_t get_Ticks_callout( v68k::processor_state& s )
{
	using namespace v68k::time;
	
	s.d(0) = guest_uptime_ticks();
	
	return rts;
}

static
int32_t get_microseconds_callout( v68k::processor_state& s )
{
	using namespace v68k::time;
	
	const uint64_t t = guest_uptime_microseconds();
	
	/*
		_Microseconds expects the low long in D0 and the high long in A0.
	*/
	
	s.a(0) = uint32_t( t >> 32 );
	s.d(0) = uint32_t( t       );
	
	return rts;
}

static
int32_t fast_memset_callout( v68k::processor_state& s )
{
	const v68k::function_code_t data_space = s.data_space();
	
	const uint32_t mem = s.a(0);
	
	const uint32_t n = s.d(0);
	const uint32_t x = s.d(1);
	
	if ( n == 0 )
	{
		return rts;
	}
	
	uint8_t* p = s.mem.translate( mem, n, data_space, mem_write );
	
	if ( p == NULL )
	{
		abort();
		return nil;  // FIXME
	}
	
	memset( p, x, n );
	
	s.mem.translate( mem, n, data_space, mem_update );
	
	return rts;
}

static
int32_t fast_memnot_callout( v68k::processor_state& s )
{
	const v68k::function_code_t data_space = s.data_space();
	
	const uint32_t mem = s.a(0);
	
	const uint32_t n = s.d(0);
	
	if ( n == 0 )
	{
		return rts;
	}
	
	uint8_t* p = s.mem.translate( mem, n, data_space, mem_write );
	
	if ( p == NULL )
	{
		abort();
		return nil;  // FIXME
	}
	
	uint8_t* q = p + n;
	
	while ( p < q )
	{
		*p++ ^= 0xFF;
	}
	
	s.mem.translate( mem, n, data_space, mem_update );
	
	return rts;
}

static
int32_t fast_rshift_callout( v68k::processor_state& s )
{
	const v68k::function_code_t data_space = s.data_space();
	
	const uint32_t a = s.a(0);
	const uint32_t b = s.a(1);
	
	const uint32_t n = s.d(0);
	const uint32_t x = s.d(1);
	
	if ( x >= 8 )
	{
		abort();
		return nil;  // FIXME
	}
	
	const uint8_t right_shift = x;
	const uint8_t left_shift  = 8 - right_shift;
	
	const uint8_t* src = s.mem.translate( a, n, data_space, mem_read );
	
	if ( src == NULL )
	{
		abort();
		return nil;  // FIXME
	}
	
	uint8_t* dst = s.mem.translate( b, n + 1, data_space, mem_write );
	
	if ( dst == NULL )
	{
		abort();
		return nil;  // FIXME
	}
	
	uint32_t n_src_bytes = n;
	
	while ( n_src_bytes-- > 0 )
	{
		const uint8_t byte = *src++;
		
		*dst++ |= byte >> right_shift;
		*dst    = byte << left_shift;
	}
	
	s.mem.translate( b, n + 1, data_space, mem_update );
	
	return rts;
}

static
int32_t fast_mempcpy_callout( v68k::processor_state& s )
{
	const v68k::function_code_t data_space = s.data_space();
	
	const uint32_t dst = s.a(0);
	const uint32_t src = s.a(1);
	
	const uint32_t n = s.d(0);
	
	const uint8_t* p = s.mem.translate( src, n, data_space, mem_read );
	
	if ( p == NULL )
	{
		abort();
		return nil;  // FIXME
	}
	
	uint8_t* q = s.mem.translate( dst, n, data_space, mem_write );
	
	if ( q == NULL )
	{
		abort();
		return nil;  // FIXME
	}
	
	memmove( q, p, n );
	
	s.mem.translate( dst, n, data_space, mem_update );
	
	s.a(0) += n;
	s.a(1) += n;
	
	return rts;
}

static
int32_t mem_test_callout( v68k::processor_state& s )
{
	uint32_t addr = s.a(0);
	uint32_t size = s.d(0);
	
	function_code_t fc = s.data_space();
	
	bool ok = s.mem.translate( addr, size, fc, mem_read ) != NULL;
	
	s.d(0) = ok;
	
	return rts;
}

static
int32_t fast_memcmp_callout( v68k::processor_state& s )
{
	const v68k::function_code_t data_space = s.data_space();
	
	const uint32_t one = s.a(0);
	const uint32_t two = s.a(1);
	const uint32_t len = s.d(0);
	
	const uint8_t* p = s.mem.translate( one, len, data_space, mem_read );
	
	if ( p == NULL )
	{
		abort();
		return nil;  // FIXME
	}
	
	const uint8_t* q = s.mem.translate( two, len, data_space, mem_read );
	
	if ( q == NULL )
	{
		abort();
		return nil;  // FIXME
	}
	
	s.d(0) = memcmp( p, q, len );
	
	return rts;
}

static
int32_t system_call_callout( v68k::processor_state& s )
{
	op_result result = bridge_call( s );
	
	if ( result >= 0 )
	{
		return rts;
	}
	else if ( result == Illegal_instruction )
	{
		FATAL = "Unimplemented system call: ", s.d(0);
	}
	
	return nil;
}

static
int32_t microseconds_callout( v68k::processor_state& s )
{
	const v68k::function_code_t data_space = s.data_space();
	
	uint32_t sp = s.a(7);
	
	uint32_t result_address;
	
	if ( ! s.mem.get_long( sp + 4, result_address, data_space ) )
	{
		return v68k::Bus_error;
	}
	
	using namespace v68k::time;
	
	uint64_t t = guest_uptime_microseconds();
	
	if ( ! s.mem.put_long( result_address, high_long( t ), data_space ) )
	{
		return v68k::Bus_error;
	}
	
	if ( ! s.mem.put_long( result_address + 4, low_long( t ), data_space ) )
	{
		return v68k::Bus_error;
	}
	
	return rts;
}

static
int32_t BlockMove_callout( v68k::processor_state& s )
{
	const v68k::function_code_t data_space = s.data_space();
	
	const uint32_t src = s.a(0);
	const uint32_t dst = s.a(1);
	
	const uint32_t n = s.d(0);
	
	const uint8_t* p = s.mem.translate( src, n, data_space, mem_read );
	
	if ( p == NULL )
	{
		abort();
		return nil;  // FIXME
	}
	
	uint8_t* q = s.mem.translate( dst, n, data_space, mem_write );
	
	if ( q == NULL )
	{
		abort();
		return nil;  // FIXME
	}
	
	memmove( q, p, n );
	
	s.mem.translate( dst, n, data_space, mem_update );
	
	s.d(0) = noErr;
	
	return rts;
}

#define OSTYPE(a, b, c, d)  ((a) << 24 | (b) << 16 | (c) << 8 | (d))

static
int32_t Gestalt_callout( v68k::processor_state& s )
{
	const int32_t gestaltUndefSelectorErr = -5551;
	
	const uint32_t selector = s.d(0);
	
	uint32_t value = 0;
	int32_t result = 0;
	
	switch ( selector )
	{
		case OSTYPE('v', '6', '8', 'k'):  // 'v68k'
		case OSTYPE('m', 'a', 'c', 'h'):  // 'mach'
		//	value = 0;
			break;
		
		case OSTYPE('p', 'r', 'o', 'c'):  // 'proc'
			value = (s.model >> 4) + 1;
			break;
		
		default:
			result = gestaltUndefSelectorErr;
			
			NOTICE = "Undefined Gestalt selector ", code32_t( selector );
			break;
	}
	
	s.sr.nzvc = result ? 0x8 : 0x4;  // either N or Z
	
	s.d(0) = result;
	s.a(0) = value;
	
	return rts;
}


static
int32_t bus_error_callout( v68k::processor_state& s )
{
	WRITE_ERR( "Bus Error" );
	
	dump_and_raise( s, SIGSEGV );
	
	return nil;
}

static
int32_t address_error_callout( v68k::processor_state& s )
{
	WRITE_ERR( "Address Error" );
	
	dump_and_raise( s, SIGBUS );
	
	return nil;
}

static
int32_t illegal_instruction_callout( v68k::processor_state& s )
{
	WRITE_ERR( "Illegal Instruction" );
	
	dump_and_raise( s, SIGILL );
	
	return nil;
}

static
int32_t division_by_zero_callout( v68k::processor_state& s )
{
	WRITE_ERR( "Division By Zero" );
	
	dump_and_raise( s, SIGFPE );
	
	return nil;
}

static
int32_t chk_trap_callout( v68k::processor_state& s )
{
	WRITE_ERR( "CHK range exceeded" );
	
	dump_and_raise( s, SIGFPE );
	
	return nil;
}

static
int32_t trapv_trap_callout( v68k::processor_state& s )
{
	WRITE_ERR( "TRAPV on overflow" );
	
	dump_and_raise( s, SIGFPE );
	
	return nil;
}

static
int32_t privilege_violation_callout( v68k::processor_state& s )
{
	WRITE_ERR( "Privilege Violation" );
	
	dump_and_raise( s, SIGILL );
	
	return nil;
}

static
int32_t trace_exception_callout( v68k::processor_state& s )
{
	WRITE_ERR( "Trace Exception" );
	
	dump_and_raise( s, SIGTRAP );
	
	return nil;
}

static
int32_t line_A_emulator_callout( v68k::processor_state& s )
{
	WRITE_ERR( "Line A Emulator" );
	
	dump_and_raise( s, SIGILL );
	
	return nil;
}

static
int32_t line_F_emulator_callout( v68k::processor_state& s )
{
	WRITE_ERR( "Line F Emulator" );
	
	dump_and_raise( s, SIGILL );
	
	return nil;
}

#ifndef SIGSTKFLT
#define SIGSTKFLT SIGSEGV
#endif

#ifndef SIGFMT
#define SIGFMT SIGSTKFLT
#endif

static
int32_t format_error_callout( v68k::processor_state& s )
{
	WRITE_ERR( "Format Error" );
	
	dump_and_raise( s, SIGFMT );
	
	return nil;
}

static
int32_t sigint_interrupt_callout( v68k::processor_state& s )
{
	WRITE_ERR( "SIGINT interrupt" );
	
	signal( SIGINT, SIG_DFL );
	
	dump_and_raise( s, SIGINT );
	
	return nil;
}


static const function_type the_callouts[] =
{
	&no_op_callout,
	&load_callout,
	&enter_supervisor_mode_callout,
	&module_suspend_callout,
	
	&set_trace_mode_callout,
	&set_trace_mode_callout,
	&set_trace_mode_callout,
	&set_trace_mode_callout,
	
	&ScrnBitMap_callout,
	&lock_screen_callout,
	&unlock_screen_callout,
	NULL,
	
	&alloc_callout,
	&dealloc_callout,
	NULL,
	NULL,
	
	&get_Ticks_callout,
	&get_microseconds_callout,
	NULL,
	NULL,
	
	&fast_memset_callout,
	&fast_memnot_callout,
	&fast_rshift_callout,
	&fast_mempcpy_callout,
	
	&mem_test_callout,
	NULL,
	NULL,
	NULL,
	
	&fast_memcmp_callout,
	NULL,
	NULL,
	NULL,
	
	&system_call_callout,
	&microseconds_callout,
	
	&bus_error_callout,
	&address_error_callout,
	&illegal_instruction_callout,
	&division_by_zero_callout,
	&chk_trap_callout,
	&trapv_trap_callout,
	&privilege_violation_callout,
	&trace_exception_callout,
	&line_A_emulator_callout,
	&line_F_emulator_callout,
	&format_error_callout,
	
	&sigint_interrupt_callout,
	
	&unimplemented_trap_callout,
	&BlockMove_callout,
	&Gestalt_callout,
	&unimplemented_callout
};


int32_t bridge( v68k::processor_state& s )
{
	const int32_t pc = s.pc();
	
	const uint32_t call_number = pc / -2 - 1;
	
	const size_t n_callouts = sizeof the_callouts / sizeof the_callouts[0];
	
	if ( call_number < n_callouts )
	{
		const function_type f = the_callouts[ call_number ];
		
		if ( f != NULL )
		{
			return f( s );
		}
	}
	
	return nil;
}

}  // namespace callout
}  // namespace v68k
