/*
	fetch.cc
	--------
*/

#include "v68k/fetch.hh"

// v68k
#include "v68k/memory.hh"
#include "v68k/registers.hh"


namespace v68k
{
	
	uint16_t fetch_instruction_word( registers& regs, const memory& mem )
	{
		const uint16_t word = mem.get_instruction_word( regs.pc );
		
		regs.pc += 2;
		
		return word;
	}
	
	
	uint32_t fetch_word_displacement( registers& regs, const memory& mem )
	{
		const int16_t word = fetch_instruction_word( regs, mem );
		
		return int32_t( word );
	}
	
	uint32_t fetch_longword_displacement( registers& regs, const memory& mem )
	{
		const uint32_t high = fetch_instruction_word( regs, mem );
		
		return high << 16 | fetch_instruction_word( regs, mem );
	}
	
	
	uint32_t fetch_major_register_id( registers& regs, const memory& )
	{
		return regs.op >> 9 & 0x000F;
	}
	
	uint32_t fetch_minor_register_id( registers& regs, const memory& )
	{
		return regs.op & 0x000F;
	}
	
	
	uint32_t fetch_major_register_num( registers& regs, const memory& )
	{
		return regs.op >> 9 & 0x0007;
	}
	
	uint32_t fetch_minor_register_num( registers& regs, const memory& )
	{
		return regs.op & 0x0007;
	}
	
	
	uint32_t fetch_MOVEP_opmode( registers& regs, const memory& )
	{
		return regs.op >> 6 & 0x0007;
	}
	
	
	uint32_t fetch_MOVEQ_data( registers& regs, const memory& )
	{
		return int32_t( int8_t( regs.op & 0x00ff ) );
	}
	
	
	uint32_t fetch_EXG_opmode( registers& regs, const memory& )
	{
		return regs.op >> 3 & 0x001f;
	}
	
}

