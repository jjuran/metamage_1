/*
	bridge.cc
	---------
*/

#include "callback/bridge.hh"

// Standard C
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


static const function_type the_callbacks[] =
{
	&unimplemented_callback,
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

