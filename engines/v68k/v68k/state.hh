/*
	state.hh
	--------
*/

#ifndef V68K_STATE_HH
#define V68K_STATE_HH

// C99
#include <stdint.h>

// v68k
#include "v68k/memory.hh"
#include "v68k/registers.hh"


namespace v68k
{
	
	enum processor_condition
	{
		startup = 0,
		
		halted = -1,
		
		normal = 1
	};
	
	struct processor_state
	{
		registers regs;
		
		const memory mem;
		
		processor_condition condition;
		
		uint16_t opcode;  // current instruction opcode
		
		processor_state( uint8_t* mem_base, uint32_t mem_size );
	};
	
}

#endif

