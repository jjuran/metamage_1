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
	
	fetcher fetches_effective_control_address[] =
	{
		&fetch_effective_control_address,
		0  // NULL
	};
	
	fetcher fetches_data_at_0007[] =
	{
		&fetch_data_at_0007,
		0  // NULL
	};
	
	
	fetcher fetches_MOVEP[] =
	{
		&fetch_MOVEP_opmode,
		&FETCH_MAJOR_REGISTER_NUM,
		&FETCH_MINOR_REGISTER_NUM,
		&fetch_word_displacement,
		0  // NULL
	};
	
	
	fetcher fetches_LINK[] =
	{
		&FETCH_MINOR_REGISTER_NUM,
		&fetch_word_displacement,
		0  // NULL
	};
	
	fetcher fetches_LINK_L[] =
	{
		&FETCH_MINOR_REGISTER_NUM,
		&fetch_longword,
		0  // NULL
	};
	
	
	fetcher fetches_STOP[] =
	{
		&fetch_unsigned_word,
		0  // NULL
	};
	
	
	fetcher fetches_branch_short[] =
	{
		&fetch_pc,
		&fetch_signed_data_at_00FF,
		0  // NULL
	};
	
	fetcher fetches_branch[] =
	{
		&fetch_pc,
		&fetch_word_displacement,
		0  // NULL
	};
	
	fetcher fetches_branch_long[] =
	{
		&fetch_pc,
		&fetch_longword,
		0  // NULL
	};
	
	
	fetcher fetches_MOVEQ[] =
	{
		&FETCH_MAJOR_REGISTER_NUM,
		&fetch_signed_data_at_00FF,
		0  // NULL
	};
	
	
	fetcher fetches_EXG[] =
	{
		&fetch_EXG_opmode,
		&FETCH_MAJOR_REGISTER_NUM,
		&FETCH_MINOR_REGISTER_ID,
		0  // NULL
	};
	
}

