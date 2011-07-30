/*
	fetches.cc
	----------
*/

#include "v68k/fetches.hh"

// v68k
#include "v68k/fetch.hh"


namespace v68k
{
	
	// General
	
	fetcher fetches_none[] =
	{
		0  // NULL
	};
	
	fetcher fetches_ANY_minor_register_num[] =
	{
		&fetch_minor_register_num,
		0  // NULL
	};
	
	
	fetcher fetches_MOVEP[] =
	{
		&fetch_MOVEP_opmode,
		&fetch_major_register_num,
		&fetch_minor_register_num,
		&fetch_word_displacement,
		0  // NULL
	};
	
	
	fetcher fetches_LINK[] =
	{
		&fetch_minor_register_num,
		&fetch_word_displacement,
		0  // NULL
	};
	
	
	fetcher fetches_MOVEQ[] =
	{
		&fetch_major_register_num,
		&fetch_MOVEQ_data,
		0  // NULL
	};
	
	
	fetcher fetches_EXG[] =
	{
		&fetch_EXG_opmode,
		&fetch_major_register_num,
		&fetch_minor_register_id,
		0  // NULL
	};
	
}

