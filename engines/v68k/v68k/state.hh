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
	
	enum processor_model
	{
		mc68000 = 0,
		mc68010 = 1,
		mc68020 = 2,
		mc68030 = 3,
		mc68040 = 4
	};
	
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
		
		const processor_model model;
		
		processor_condition condition;
		
		uint16_t opcode;  // current instruction opcode
		
		processor_state( processor_model model, uint8_t* mem_base, uint32_t mem_size );
		
		uint16_t get_SR() const;
		
		void set_SR( uint16_t new_sr );
	};
	
}

#endif

