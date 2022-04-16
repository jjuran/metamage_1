/*
	ops.hh
	------
*/

#ifndef V68K_OPS_HH
#define V68K_OPS_HH

// v68k
#include "v68k/op.hh"


namespace v68k
{
	
	struct op_params;
	struct processor_state;
	
	
	op_result sign_extend_byte( processor_state& state, op_params& pb );
	op_result sign_extend_word( processor_state& state, op_params& pb );
	
}

#endif
