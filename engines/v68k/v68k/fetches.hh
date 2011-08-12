/*
	fetches.hh
	----------
*/

#ifndef V68K_FETCHES_HH
#define V68K_FETCHES_HH

// v68k
#include "v68k/fetcher.hh"


namespace v68k
{
	
	extern fetcher fetches_none[];
	
	extern fetcher fetches_effective_control_address[];
	
	extern fetcher fetches_immediate[];
	
	extern fetcher fetches_data_at_0007[];
	
	extern fetcher fetches_MOVEP[];
	
	extern fetcher fetches_MOVE_B_to_Dn[];
	extern fetcher fetches_MOVE_B      [];
	
	extern fetcher fetches_MOVE_L_to_Rn[];
	extern fetcher fetches_MOVE_L      [];
	
	extern fetcher fetches_MOVE_W_to_Rn[];
	extern fetcher fetches_MOVE_W      [];
	
	extern fetcher fetches_LEA[];
	
	extern fetcher fetches_MOVE_from_SR[];
	
	extern fetcher fetches_MOVE_to_SR[];
	
	extern fetcher fetches_TRAP[];
	
	extern fetcher fetches_LINK_L[];
	
	extern fetcher fetches_LINK[];
	
	extern fetcher fetches_unsigned_word[];
	
	extern fetcher fetches_branch_short[];
	extern fetcher fetches_branch      [];
	extern fetcher fetches_branch_long [];
	
	extern fetcher fetches_MOVEQ[];
	
	extern fetcher fetches_EXG[];
	
}

#endif

