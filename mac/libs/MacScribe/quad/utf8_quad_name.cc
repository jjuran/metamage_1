/*
	utf8_quad_name.cc
	-----------------
*/

#include "quad/utf8_quad_name.hh"

// plus
#include "plus/mac_utf8.hh"


namespace MacScribe
{
	
	quad_t parse_utf8_quad_name( const plus::string& name )
	{
		return parse_quad_name( plus::mac_from_utf8( name ) );
	}
	
	plus::string make_utf8_quad_name( quad_t q )
	{
		return plus::utf8_from_mac( make_quad_name( q ) );
	}
	
}

