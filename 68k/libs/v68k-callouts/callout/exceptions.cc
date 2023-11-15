/*
	exceptions.cc
	-------------
*/

#include "callout/exceptions.hh"

// POSIX
#include <unistd.h>

// Standard C
#include <signal.h>

// must
#include "must/write.h"

// v68k-ioutils
#include "ioutils/print_register_dump.hh"


#ifdef __MWERKS__
#pragma exceptions off
#endif


#define ERR_MSG( msg )  "v68k: exception: " msg "\n"

#define STRLEN( s )  (sizeof "" s - 1)

#define STR_LEN( s )  "" s, (sizeof s - 1)

#define WRITE_ERR( msg )  must_write( STDERR_FILENO, STR_LEN( ERR_MSG( msg ) ) )


namespace v68k    {
namespace callout {

enum
{
	rts = 0x4E75,
	nil = 0
};

static
void dump_and_raise( const v68k::processor_state& s, int signo )
{
	using v68k::ioutils::print_register_dump;
	
	print_register_dump( s.regs, s.get_SR() );
	
	raise( signo );
}

static char hex[] =
{
	'0', '1', '2', '3',
	'4', '5', '6', '7',
	'8', '9', 'A', 'B',
	'C', 'D', 'E', 'F'
};

#define UNIMPLEMENTED_TRAP_PREFIX  "v68k: exception: Unimplemented Mac trap: "

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


int32_t bus_error_callout( v68k::processor_state& s )
{
	WRITE_ERR( "Bus Error" );
	
	dump_and_raise( s, SIGSEGV );
	
	return nil;
}

int32_t address_error_callout( v68k::processor_state& s )
{
	WRITE_ERR( "Address Error" );
	
	dump_and_raise( s, SIGBUS );
	
	return nil;
}

int32_t illegal_instruction_callout( v68k::processor_state& s )
{
	WRITE_ERR( "Illegal Instruction" );
	
	dump_and_raise( s, SIGILL );
	
	return nil;
}

int32_t division_by_zero_callout( v68k::processor_state& s )
{
	WRITE_ERR( "Division By Zero" );
	
	dump_and_raise( s, SIGFPE );
	
	return nil;
}

int32_t chk_trap_callout( v68k::processor_state& s )
{
	WRITE_ERR( "CHK range exceeded" );
	
	dump_and_raise( s, SIGFPE );
	
	return nil;
}

int32_t trapv_trap_callout( v68k::processor_state& s )
{
	WRITE_ERR( "TRAPV on overflow" );
	
	dump_and_raise( s, SIGFPE );
	
	return nil;
}

int32_t privilege_violation_callout( v68k::processor_state& s )
{
	WRITE_ERR( "Privilege Violation" );
	
	dump_and_raise( s, SIGILL );
	
	return nil;
}

int32_t trace_exception_callout( v68k::processor_state& s )
{
	WRITE_ERR( "Trace Exception" );
	
	dump_and_raise( s, SIGTRAP );
	
	return nil;
}

int32_t line_A_emulator_callout( v68k::processor_state& s )
{
	WRITE_ERR( "Line A Emulator" );
	
	dump_and_raise( s, SIGILL );
	
	return nil;
}

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

int32_t format_error_callout( v68k::processor_state& s )
{
	WRITE_ERR( "Format Error" );
	
	dump_and_raise( s, SIGFMT );
	
	return nil;
}

int32_t sigint_interrupt_callout( v68k::processor_state& s )
{
	WRITE_ERR( "SIGINT interrupt" );
	
	signal( SIGINT, SIG_DFL );
	
	dump_and_raise( s, SIGINT );
	
	return nil;
}

}  // namespace callout
}  // namespace v68k
