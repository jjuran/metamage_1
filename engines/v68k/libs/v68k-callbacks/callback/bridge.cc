/*
	bridge.cc
	---------
*/

#include "callback/bridge.hh"

// POSIX
#include <unistd.h>

// Standard C
#include <signal.h>
#include <stdlib.h>


#pragma exceptions off


namespace v68k     {
namespace callback {

enum
{
	nil = 0
};

typedef uint32_t (*function_type)( v68k::emulator& emu );


static uint32_t unimplemented_callback( v68k::emulator& emu )
{
	abort();
	
	// Not reached
	return nil;
}


#define ERR_MSG( msg )  "v68k: exception: " msg "\n"

#define STR_LEN( s )  "" s, (sizeof s - 1)

#define WRITE_ERR( msg )  write( STDERR_FILENO, STR_LEN( ERR_MSG( msg ) ) )

static uint32_t illegal_instruction_callback( v68k::emulator& emu )
{
	WRITE_ERR( "Illegal Instruction" );
	
	raise( SIGILL );
	
	return nil;
}

static uint32_t privilege_violation_callback( v68k::emulator& emu )
{
	WRITE_ERR( "Privilege Violation" );
	
	raise( SIGILL );
	
	return nil;
}

static uint32_t line_F_emulator_callback( v68k::emulator& emu )
{
	WRITE_ERR( "Line F Emulator" );
	
	raise( SIGILL );
	
	return nil;
}


static const function_type the_callbacks[] =
{
	&unimplemented_callback,
	&illegal_instruction_callback,
	&privilege_violation_callback,
	&line_F_emulator_callback,
	NULL
};


uint32_t bridge( v68k::emulator& emu )
{
	const int32_t pc = emu.regs.pc;
	
	const uint32_t call_number = pc / -2 - 1;
	
	const size_t n_callbacks = sizeof the_callbacks / sizeof the_callbacks[0];
	
	if ( call_number < n_callbacks )
	{
		const function_type f = the_callbacks[ call_number ];
		
		if ( f != NULL )
		{
			return f( emu );
		}
	}
	
	return nil;
}

}  // namespace v68k
}  // namespace callback

