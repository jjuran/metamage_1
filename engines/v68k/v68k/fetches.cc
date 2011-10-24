/*
	fetches.cc
	----------
*/

#include "v68k/fetches.hh"

// v68k
#include "v68k/effective_address.hh"
#include "v68k/fetch.hh"


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
	
	fetcher fetches_immediate[] =
	{
		&fetch_sized_immediate_data,
		&fetch_effective_address,
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
		&fetch_data_at_0E00,  // register number or data
		&fetch_effective_address,
		0  // NULL
	};
	
	fetcher fetches_EOR[] =
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
	
	
	fetcher fetches_static_bit_op[] =
	{
		&fetch_unsigned_word,
		&fetch_effective_address,
		0  // NULL
	};
	
	
	fetcher fetches_MOVEP[] =
	{
		&FETCH_MAJOR_REGISTER_NUM,
		&fetch_MOVEP_address,
		0  // NULL
	};
	
	
	fetcher fetches_MOVES[] =
	{
		&fetch_unsigned_word,
		&fetch_effective_address,
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
		&fetch_signed_word,
		0  // NULL
	};
	
	fetcher fetches_LINK_L[] =
	{
		&FETCH_MINOR_REGISTER_NUM,
		&fetch_longword,
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
		&fetch_zero,
		&fetch_sized_data_at_effective_address,
		0  // NULL
	};
	
	
	fetcher fetches_ADDQ[] =
	{
		&fetch_ADDQ_data,
		&fetch_effective_address,
		0  // NULL
	};
	
	
	fetcher fetches_Scc[] =
	{
		&fetch_cc,
		&fetch_effective_address,
		0  // NULL
	};
	
	
	fetcher fetches_branch_short[] =
	{
		&fetch_pc,
		&fetch_signed_data_at_00FF,
		&fetch_cc,
		0  // NULL
	};
	
	fetcher fetches_branch[] =
	{
		&fetch_pc,
		&fetch_signed_word,
		&fetch_cc,
		0  // NULL
	};
	
	fetcher fetches_branch_long[] =
	{
		&fetch_pc,
		&fetch_longword,
		&fetch_cc,
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
		&fetch_sized_data_at_effective_address,
		&fetch_sized_data_from_major_register,
		0  // NULL
	};
	
	fetcher fetches_CMPA[] =
	{
		&fetch_sized_data_at_effective_address,
		&fetch_A_data_from_major_register,
		0  // NULL
	};
	
	
	fetcher fetches_EXG[] =
	{
		&fetch_EXG_first_reg,
		&FETCH_MINOR_REGISTER_ID,
		0  // NULL
	};
	
	
	fetcher fetches_ADD_to_Dn[] =
	{
		&fetch_sized_data_at_effective_address,
		&fetch_data_at_0E00,  // register number
		0  // NULL
	};
	
	fetcher fetches_ADDA[] =
	{
		&fetch_sized_data_at_effective_address,
		&FETCH_MAJOR_REGISTER_ID,  // register id (An for line D)
		0  // NULL
	};
	
	fetcher fetches_ADD[] =
	{
		&fetch_sized_data_from_major_register,
		&fetch_effective_address,
		0  // NULL
	};
	
	
	fetcher fetches_bit_shift[] =
	{
		&fetch_bit_shift_count,
		&FETCH_MINOR_REGISTER_NUM,
		0  // NULL
	};
	
}

