/*
	effective_address.hh
	--------------------
*/

#ifndef V68K_EFFECTIVEADDRESS_HH
#define V68K_EFFECTIVEADDRESS_HH

// C99
#include <stdint.h>

// v68k
#include "v68k/op.hh"


namespace v68k
{
	
	struct op_params;
	struct processor_state;
	
	
	op_result fetch_effective_address( processor_state& state, uint16_t mode, uint16_t n, op_params& pb );
	
}

#endif
