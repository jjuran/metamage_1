/*
	effective_address.hh
	--------------------
*/

#ifndef V68K_EFFECTIVEADDRESS_HH
#define V68K_EFFECTIVEADDRESS_HH

// C99
#include <stdint.h>


namespace v68k
{
	
	struct processor_state;
	
	
	uint32_t fetch_effective_control_address( processor_state& state );
	
}

#endif

