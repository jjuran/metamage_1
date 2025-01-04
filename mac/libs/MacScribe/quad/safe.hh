/*
	safe.hh
	-------
*/

#ifndef MACSCRIBE_QUAD_SAFE_HH
#define MACSCRIBE_QUAD_SAFE_HH

// Standard C
#include <stdint.h>


namespace MacScribe
{
	
	typedef uint32_t quad_t;
	
	bool is_safe_quad( quad_t q );
	
}

#endif
