/*
	fetch.cc
	--------
*/

#include "v68k/fetch.hh"

// v68k
#include "v68k/state.hh"


namespace v68k
{
	
	uint16_t fetch_instruction_word( processor_state& s )
	{
		const uint16_t word = s.mem.get_instruction_word( s.regs.pc );
		
		s.regs.pc += 2;
		
		return word;
	}
	
	
	uint32_t fetch_word_displacement( processor_state& s )
	{
		const int16_t word = fetch_instruction_word( s );
		
		return int32_t( word );
	}
	
	uint32_t fetch_longword_displacement( processor_state& s )
	{
		const uint32_t high = fetch_instruction_word( s );
		
		return high << 16 | fetch_instruction_word( s );
	}
	
	
	uint32_t fetch_major_register_id( processor_state& s )
	{
		return s.regs.op >> 9 & 0x000F;
	}
	
	uint32_t fetch_minor_register_id( processor_state& s )
	{
		return s.regs.op & 0x000F;
	}
	
	
	uint32_t fetch_major_register_num( processor_state& s )
	{
		return s.regs.op >> 9 & 0x0007;
	}
	
	uint32_t fetch_minor_register_num( processor_state& s )
	{
		return s.regs.op & 0x0007;
	}
	
	
	uint32_t fetch_MOVEP_opmode( processor_state& s )
	{
		return s.regs.op >> 6 & 0x0007;
	}
	
	
	uint32_t fetch_MOVEQ_data( processor_state& s )
	{
		return int32_t( int8_t( s.regs.op & 0x00ff ) );
	}
	
	
	uint32_t fetch_EXG_opmode( processor_state& s )
	{
		return s.regs.op >> 3 & 0x001f;
	}
	
}

