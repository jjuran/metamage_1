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
		mc68000 = 0x00,
		mc68010 = 0x10,
		mc68020 = 0x20,
		mc68030 = 0x30,
		mc68040 = 0x40
	};
	
	enum processor_condition
	{
		startup = 0,
		
		halted  = -1,  // double bus fault
		stopped = -2,  // STOP instruction
		
		bkpt_0 = 0xF0,
		bkpt_1 = 0xF1,
		bkpt_2 = 0xF2,
		bkpt_3 = 0xF3,
		bkpt_4 = 0xF4,
		bkpt_5 = 0xF5,
		bkpt_6 = 0xF6,
		bkpt_7 = 0xF7,
		
		bkpt_mask = 0xF8,
		
		finished = 2,
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

