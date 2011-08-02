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
	
	uint32_t fetch_effective_byte_address( processor_state& state );
	uint32_t fetch_effective_word_address( processor_state& state );
	uint32_t fetch_effective_long_address( processor_state& state );
	
	uint32_t fetch_2nd_effective_byte_address( processor_state& state );
	uint32_t fetch_2nd_effective_word_address( processor_state& state );
	uint32_t fetch_2nd_effective_long_address( processor_state& state );
	
	uint32_t fetch_byte_from_effective_address( processor_state& state );
	uint32_t fetch_word_from_effective_address( processor_state& state );
	uint32_t fetch_long_from_effective_address( processor_state& state );
	
}

#endif

