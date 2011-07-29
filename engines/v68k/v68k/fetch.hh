/*
	fetch.hh
	--------
*/

#ifndef V68K_FETCH_HH
#define V68K_FETCH_HH

// C99
#include <stdint.h>


namespace v68k
{
	
	struct registers;
	
	class memory;
	
	
	uint16_t fetch_instruction_word( registers& regs, const memory& mem );
	
	
	uint32_t fetch_word_displacement( registers& regs, const memory& mem );
	
	uint32_t fetch_longword_displacement( registers& regs, const memory& mem );
	
	// 4-bit value, high bit is 0 for D and 1 for A
	uint32_t fetch_major_register_id( registers& regs, const memory& );
	uint32_t fetch_minor_register_id( registers& regs, const memory& );
	
	// 3-bit value, register number only
	uint32_t fetch_major_register_num( registers& regs, const memory& );
	uint32_t fetch_minor_register_num( registers& regs, const memory& );
	
	uint32_t fetch_MOVEP_opmode( registers& regs, const memory& );
	
	uint32_t fetch_MOVEQ_data( registers& regs, const memory& );
	
	uint32_t fetch_EXG_opmode( registers& regs, const memory& );
	
}

#endif

