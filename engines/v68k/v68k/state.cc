/*
	state.cc
	--------
*/

#include "v68k/state.hh"


namespace v68k
{
	
	processor_state::processor_state( uint8_t* mem_base, uint32_t mem_size )
	:
		mem( mem_base, mem_size ),
		condition()
	{
	}
	
}

