/*
	utf8_quad_name.hh
	-----------------
*/

#ifndef MACSCRIBE_QUAD_UTF8QUADNAME_HH
#define MACSCRIBE_QUAD_UTF8QUADNAME_HH

// MacScribe
#include "quad/quad_name.hh"


namespace MacScribe
{
	
	quad_t parse_utf8_quad_name( const plus::string& name );
	
	plus::string make_utf8_quad_name( quad_t q );
	
}

#endif

