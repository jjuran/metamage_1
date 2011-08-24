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
	
	
	uint32_t fetch_pc( processor_state& state );
	
	uint32_t fetch_unsigned_word( processor_state& state );
	
	uint32_t fetch_signed_word( processor_state& state );
	
	uint32_t fetch_longword( processor_state& state );
	
	uint32_t fetch_size_code( processor_state& state );
	
	uint32_t fetch_A_size_code( processor_state& state );
	
	uint32_t fetch_sized_immediate_data( processor_state& state );
	
	uint32_t fetch_sized_effective_address( processor_state& state );
	
	uint32_t fetch_sized_data_at_effective_address( processor_state& state );
	
	uint32_t fetch_sized_data_from_major_register( processor_state& state );
	
	uint32_t fetch_A_data_at_effective_address( processor_state& state );
	
	uint32_t fetch_A_data_from_major_register( processor_state& state );
	
	uint32_t fetch_data_at_1E00( processor_state& state );
	uint32_t fetch_data_at_000F( processor_state& state );
	
	uint32_t fetch_data_at_0E00( processor_state& state );
	uint32_t fetch_data_at_0007( processor_state& state );
	
	uint32_t fetch_2nd_ea_register_id( processor_state& state );
	
	uint32_t fetch_MOVEP_opmode( processor_state& state );
	
	uint32_t fetch_cc( processor_state& state );
	
	uint32_t fetch_signed_data_at_00FF( processor_state& state );
	
	uint32_t fetch_EXG_opmode( processor_state& state );
	
}

#endif

