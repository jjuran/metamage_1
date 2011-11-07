/*
	load_store.hh
	-------------
*/

#ifndef V68K_LOADSTORE_HH
#define V68K_LOADSTORE_HH

// C99
#include <stdint.h>


namespace v68k
{
	
	struct processor_state;
	struct op_params;
	
	
	bool load( const processor_state& s, op_params& pb, int flags );
	
	bool store( processor_state& s, uint32_t loc, uint32_t data, int flags );
	
}

#endif

