/*
	quad_name.hh
	------------
*/

#ifndef MACSCRIBE_QUAD_QUADNAME_HH
#define MACSCRIBE_QUAD_QUADNAME_HH

// Standard C
#include <stdint.h>

// plus
#include "plus/string.hh"


namespace MacScribe
{
	
	typedef uint32_t quad_t;
	
	class invalid_quad_name {};
	
	quad_t parse_quad_name( const char* name, unsigned size );
	
	quad_t parse_quad_name( const plus::string& name );
	
	plus::string make_quad_name( quad_t q );
	
}

#endif

