/*
	fetch.hh
	--------
*/

#ifndef V68K_FETCH_HH
#define V68K_FETCH_HH

// C99
#include <stdint.h>

// v68k
#include "v68k/op.hh"


namespace v68k
{
	
	struct processor_state;
	struct op_params;
	
	
	op_result fetch_instruction_word( processor_state& s, uint16_t& longword );
	op_result fetch_instruction_long( processor_state& s, uint32_t& longword );
	
	op_result fetch_instruction_word_signed( processor_state& s, uint32_t& longword );
	
	
	op_result fetch_effective_address( processor_state& state, op_params& pb );
	
	op_result fetch_2nd_effective_address( processor_state& state, op_params& pb );
	
	
	op_result fetch_zero( processor_state& state, op_params& pb );
	
	op_result fetch_one( processor_state& state, op_params& pb );
	
	op_result fetch_ones( processor_state& state, op_params& pb );
	
	op_result fetch_unsigned_word( processor_state& state, op_params& pb );
	
	op_result fetch_signed_word( processor_state& state, op_params& pb );
	
	op_result fetch_sized_immediate_data( processor_state& state, op_params& pb );
	
	op_result fetch_sized_immediate_signed_data( processor_state& state, op_params& pb );
	
	op_result fetch_sized_data_at_effective_address( processor_state& state, op_params& pb );
	
	op_result fetch_sized_data_from_major_register( processor_state& state, op_params& pb );
	
	op_result fetch_bit_number_from_major_register( processor_state& state, op_params& pb );
	
	op_result fetch_A_data_from_major_register( processor_state& state, op_params& pb );
	
	op_result fetch_data_at_1E00( processor_state& state, op_params& pb );
	op_result fetch_data_at_000F( processor_state& state, op_params& pb );
	
	op_result fetch_data_at_0E00( processor_state& state, op_params& pb );
	op_result fetch_data_at_0007( processor_state& state, op_params& pb );
	
	op_result fetch_data_at_0001( processor_state& state, op_params& pb );
	
	op_result fetch_MOVEP_address( processor_state& state, op_params& pb );
	
	op_result fetch_MOVEM_update( processor_state& state, op_params& pb );
	
	op_result fetch_ADDQ_data( processor_state& state, op_params& pb );
	
	op_result fetch_conditional( processor_state& state, op_params& pb );
	
	op_result fetch_signed_data_at_00FF( processor_state& state, op_params& pb );
	
	op_result fetch_CMPM( processor_state& state, op_params& pb );
	
	op_result fetch_EXG_first_reg( processor_state& state, op_params& pb );
	
	op_result fetch_ADDX_predecrement( processor_state& state, op_params& pb );
	
	op_result fetch_bit_shift_count( processor_state& state, op_params& pb );
	
	
	op_result force_long_sized( processor_state& state, op_params& pb );
	
	op_result assign_first_to_second( processor_state& state, op_params& pb );
	
	op_result add_first_to_address( processor_state& state, op_params& pb );
	
	op_result add_X_to_first( processor_state& state, op_params& pb );
	
	op_result shift_NEG_operands( processor_state& state, op_params& pb );
	
	op_result replace_SP( processor_state& s, op_params& pb );
	
	
	op_result read_address_on_68000( processor_state& state, op_params& pb );
	
	op_result check_stack_alignment( processor_state& state, op_params& pb );
	
}

#endif
