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
	
	struct processor_state;
	
	
	uint16_t fetch_instruction_word( processor_state& state );
	
	
	uint32_t fetch_unsigned_word( processor_state& state );
	
	uint32_t fetch_word_displacement( processor_state& state );
	
	uint32_t fetch_longword_displacement( processor_state& state );
	
	// 4-bit value, high bit is 0 for D and 1 for A
	uint32_t fetch_major_register_id( processor_state& state );
	uint32_t fetch_minor_register_id( processor_state& state );
	
	// 3-bit value, register number only
	uint32_t fetch_major_register_num( processor_state& state );
	uint32_t fetch_minor_register_num( processor_state& state );
	
	uint32_t fetch_MOVEP_opmode( processor_state& state );
	
	uint32_t fetch_MOVEQ_data( processor_state& state );
	
	uint32_t fetch_EXG_opmode( processor_state& state );
	
}

#endif

