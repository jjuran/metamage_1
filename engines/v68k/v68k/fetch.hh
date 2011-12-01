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
	struct op_params;
	
	
	uint16_t fetch_instruction_word( processor_state& state );
	
	inline int16_t fetch_instruction_word_signed( processor_state& state )
	{
		return fetch_instruction_word( state );
	}
	
	uint32_t fetch_longword( processor_state& state );
	
	
	void fetch_effective_address( processor_state& state, op_params& pb );
	
	void fetch_2nd_effective_address( processor_state& state, op_params& pb );
	
	
	void fetch_zero( processor_state& state, op_params& pb );
	
	void fetch_ones( processor_state& state, op_params& pb );
	
	void fetch_unsigned_word( processor_state& state, op_params& pb );
	
	void fetch_signed_word( processor_state& state, op_params& pb );
	
	void fetch_sized_immediate_data( processor_state& state, op_params& pb );
	
	void fetch_sized_immediate_signed_data( processor_state& state, op_params& pb );
	
	void fetch_sized_data_at_effective_address( processor_state& state, op_params& pb );
	
	void fetch_sized_data_from_major_register( processor_state& state, op_params& pb );
	
	void fetch_bit_number_from_major_register( processor_state& state, op_params& pb );
	
	void fetch_A_data_from_major_register( processor_state& state, op_params& pb );
	
	void fetch_data_at_1E00( processor_state& state, op_params& pb );
	void fetch_data_at_000F( processor_state& state, op_params& pb );
	
	void fetch_data_at_0E00( processor_state& state, op_params& pb );
	void fetch_data_at_0007( processor_state& state, op_params& pb );
	
	void fetch_data_at_0001( processor_state& state, op_params& pb );
	
	void fetch_MOVEP_address( processor_state& state, op_params& pb );
	
	void fetch_MOVEM_update( processor_state& state, op_params& pb );
	
	void fetch_ADDQ_data( processor_state& state, op_params& pb );
	
	void fetch_cc( processor_state& state, op_params& pb );
	
	void fetch_signed_data_at_00FF( processor_state& state, op_params& pb );
	
	void fetch_CMPM( processor_state& state, op_params& pb );
	
	void fetch_EXG_first_reg( processor_state& state, op_params& pb );
	
	void fetch_ADDX_predecrement( processor_state& state, op_params& pb );
	
	void fetch_bit_shift_count( processor_state& state, op_params& pb );
	
	
	void assign_first_to_second( processor_state& state, op_params& pb );
	
	void add_first_to_address( processor_state& state, op_params& pb );
	
	void add_X_to_first( processor_state& state, op_params& pb );
	
	void shift_NEG_operands( processor_state& state, op_params& pb );
	
	
	void read_address_on_68000( processor_state& state, op_params& pb );
	
}

#endif

