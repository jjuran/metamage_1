/*
	fetcher.hh
	----------
*/

#ifndef V68K_FETCHER_HH
#define V68K_FETCHER_HH

// v68k
#include "v68k/op.hh"


namespace v68k
{
	
	struct processor_state;
	struct op_params;
	
	
	typedef op_result (*fetcher)( processor_state& state, op_params& pb );
	
}

#endif
