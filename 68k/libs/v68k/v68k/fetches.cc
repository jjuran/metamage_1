/*
	fetches.cc
	----------
*/

#include "v68k/fetches.hh"

// v68k
#include "v68k/bitfields.hh"
#include "v68k/fetch.hh"
#include "v68k/load_store.hh"


// 4-bit value, high bit is 0 for D and 1 for A
#define FETCH_MAJOR_REGISTER_ID   fetch_data_at_1E00
#define FETCH_MINOR_REGISTER_ID   fetch_data_at_000F

// 3-bit value, register number only
#define FETCH_MAJOR_REGISTER_NUM  fetch_data_at_0E00
#define FETCH_MINOR_REGISTER_NUM  fetch_data_at_0007


namespace v68k
{
	
	// General
	
	fetcher fetches_none[] =
	{
		0  // NULL
	};
	
	fetcher fetches_effective_address[] =
	{
		&fetch_effective_address,
		0  // NULL
	};
	
	fetcher fetches_word_and_EA[] =
	{
		&fetch_unsigned_word,
		&fetch_effective_address,
		0  // NULL
	};
	
	fetcher fetches_immediate[] =
	{
		&fetch_sized_immediate_data,
		&fetch_effective_address,
		0  // NULL
	};
	
	fetcher fetches_DIV[] =
	{
		&fetch_sized_data_at_effective_address,
		&fetch_data_at_0E00,  // register number
		// src divisor is word-sized, but dst dividend is long-sized.
		&force_long_sized,
		0  // NULL
	};
	
	fetcher fetches_math_to_Dn[] =
	{
		&fetch_sized_data_at_effective_address,
		&fetch_data_at_0E00,  // register number
		0  // NULL
	};
	
	fetcher fetches_math[] =
	{
		&fetch_sized_data_from_major_register,
		&fetch_effective_address,
		0  // NULL
	};
	
	fetcher fetches_signed_word[] =
	{
		&fetch_signed_word,
		0  // NULL
	};
	
	fetcher fetches_data_at_0007[] =
	{
		&fetch_data_at_0007,
		0  // NULL
	};
	
	
	fetcher fetches_dynamic_bit_op[] =
	{
		&fetch_bit_number_from_major_register,
		&fetch_effective_address,
		0  // NULL
	};
	
	
	fetcher fetches_MOVEP[] =
	{
		&FETCH_MAJOR_REGISTER_NUM,
		&fetch_MOVEP_address,
		&fetch_signed_word,
		&add_first_to_address,
		0  // NULL
	};
	
	
	fetcher fetches_MOVE[] =
	{
		&fetch_sized_data_at_effective_address,
		&fetch_2nd_effective_address,
		0  // NULL
	};
	
	
	fetcher fetches_LEA[] =
	{
		&fetch_effective_address,
		FETCH_MAJOR_REGISTER_NUM,
		0  // NULL
	};
	
	fetcher fetches_MOVE_to_SR[] =
	{
		&fetch_sized_data_at_effective_address,
		0  // NULL
	};
	
	
	fetcher fetches_MOVEM[] =
	{
		&fetch_unsigned_word,
		&fetch_effective_address,
		&fetch_MOVEM_update,
		0  // NULL
	};
	
	
	fetcher fetches_TRAP[] =
	{
		&fetch_data_at_000F,
		0  // NULL
	};
	
	
	fetcher fetches_LINK[] =
	{
		&FETCH_MINOR_REGISTER_NUM,
		&fetch_sized_immediate_signed_data,
		0  // NULL
	};
	
	
	fetcher fetches_UNLK[] =
	{
		&FETCH_MINOR_REGISTER_NUM,
		&replace_SP,
		0  // NULL
	};
	
	
	fetcher fetches_unsigned_word[] =
	{
		&fetch_unsigned_word,
		0  // NULL
	};
	
	
	fetcher fetches_CLR[] =
	{
		&fetch_zero,
		&fetch_effective_address,
		&read_address_on_68000,
		0  // NULL
	};
	
	fetcher fetches_NEG[] =
	{
		&fetch_effective_address,
		&load,
		&shift_NEG_operands,
		0  // NULL
	};
	
	fetcher fetches_NEGX[] =
	{
		&fetch_effective_address,
		&load,
		&shift_NEG_operands,
		&add_X_to_first,
		0  // NULL
	};
	
	fetcher fetches_NOT[] =
	{
		&fetch_ones,
		&fetch_effective_address,
		0  // NULL
	};
	
	fetcher fetches_TST[] =
	{
		&fetch_sized_data_at_effective_address,
		0  // NULL
	};
	
	
	fetcher fetches_MOVEC[] =
	{
		&fetch_data_at_0001,
		&fetch_unsigned_word,
		0  // NULL
	};
	
	
	fetcher fetches_ADDQ[] =
	{
		&fetch_ADDQ_data,
		&fetch_effective_address,
		0  // NULL
	};
	
	
	fetcher fetches_DBcc[] =
	{
		&fetch_sized_immediate_signed_data,
		&fetch_conditional,
		&add_first_to_address,
		&fetch_data_at_0007,
		0  // NULL
	};
	
	fetcher fetches_Scc[] =
	{
		&fetch_conditional,
		&fetch_effective_address,
		0  // NULL
	};
	
	
	fetcher fetches_branch_short[] =
	{
		&fetch_signed_data_at_00FF,
		&fetch_conditional,
		&add_first_to_address,
		0  // NULL
	};
	
	fetcher fetches_branch[] =
	{
		&fetch_sized_immediate_signed_data,
		&fetch_conditional,
		&add_first_to_address,
		0  // NULL
	};
	
	
	fetcher fetches_MOVEQ[] =
	{
		&fetch_signed_data_at_00FF,
		&FETCH_MAJOR_REGISTER_NUM,
		0  // NULL
	};
	
	
	fetcher fetches_CMP[] =
	{
		&fetch_sized_data_from_major_register,
		&assign_first_to_second,
		&fetch_sized_data_at_effective_address,
		0  // NULL
	};
	
	fetcher fetches_CMPA[] =
	{
		&fetch_sized_data_at_effective_address,
		&fetch_A_data_from_major_register,
		&force_long_sized,
		0  // NULL
	};
	
	fetcher fetches_CMPM[] =
	{
		&fetch_CMPM,
		0  // NULL
	};
	
	
	fetcher fetches_EXG[] =
	{
		&fetch_EXG_first_reg,
		&FETCH_MINOR_REGISTER_ID,
		0  // NULL
	};
	
	
	fetcher fetches_ADDA[] =
	{
		&fetch_sized_data_at_effective_address,
		&FETCH_MAJOR_REGISTER_ID,  // register id (An for line D)
		0  // NULL
	};
	
	fetcher fetches_ADDX_Dn[] =
	{
		&fetch_sized_data_at_effective_address,
		&FETCH_MAJOR_REGISTER_NUM,
		&add_X_to_first,
		0  // NULL
	};
	
	fetcher fetches_ADDX_predec[] =
	{
		&fetch_ADDX_predecrement,
		&add_X_to_first,
		0  // NULL
	};
	
	
	fetcher fetches_bit_shift_Dn[] =
	{
		&fetch_bit_shift_count,
		&FETCH_MINOR_REGISTER_NUM,
		0  // NULL
	};
	
	fetcher fetches_bit_shift_mem[] =
	{
		&fetch_one,
		&fetch_effective_address,
		0  // NULL
	};
	
	fetcher fetches_bitfield[] =
	{
		&fetch_unsigned_word,
		&fetch_effective_address,
		&normalize_bitfield_operands,
		0  // NULL
	};
	
}
