/*
	load_store.hh
	-------------
*/

#ifndef V68K_LOADSTORE_HH
#define V68K_LOADSTORE_HH

// v68k
#include "v68k/op.hh"


namespace v68k
{
	
	struct processor_state;
	struct op_params;
	
	
	op_result load( processor_state& s, op_params& pb );
	
	bool store( processor_state& s, const op_params& pb );
	
}

#endif
