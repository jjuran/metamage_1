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
	
	extern fetcher fetches_effective_address[];
	
	extern fetcher fetches_immediate[];
	
	extern fetcher fetches_math_to_Dn[];
	
	extern fetcher fetches_EOR[];
	
	extern fetcher fetches_signed_word[];
	
	extern fetcher fetches_data_at_0007[];
	
	extern fetcher fetches_dynamic_bit_op[];
	
	extern fetcher fetches_static_bit_op[];
	
	extern fetcher fetches_MOVEP[];
	
	extern fetcher fetches_MOVES[];
	
	extern fetcher fetches_MOVE[];
	
	extern fetcher fetches_LEA[];
	
	extern fetcher fetches_MOVE_to_SR[];
	
	extern fetcher fetches_MOVEM[];
	
	extern fetcher fetches_TRAP[];
	
	extern fetcher fetches_LINK[];
	
	extern fetcher fetches_unsigned_word[];
	
	extern fetcher fetches_CLR[];
	
	extern fetcher fetches_NOT[];
	
	extern fetcher fetches_TST[];
	
	extern fetcher fetches_MOVEC[];
	
	extern fetcher fetches_ADDQ[];
	
	extern fetcher fetches_Scc[];
	
	extern fetcher fetches_branch_short[];
	extern fetcher fetches_branch      [];
	
	extern fetcher fetches_MOVEQ[];
	
	extern fetcher fetches_CMP[];
	
	extern fetcher fetches_CMPA[];
	
	extern fetcher fetches_EXG[];
	
	extern fetcher fetches_ADDA[];
	extern fetcher fetches_ADD [];
	
	extern fetcher fetches_bit_shift[];
	
}

#endif

