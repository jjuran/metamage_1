/*
	quad_name.hh
	------------
*/

#ifndef GENIE_FS_QUADNAME_HH
#define GENIE_FS_QUADNAME_HH

// Standard C
#include <stdint.h>


namespace plus
{
	
	class string;
	class var_string;
	
}

namespace Genie
{
	
	typedef uint32_t quad_t;
	
	class invalid_quad_name {};
	
	quad_t parse_quad_name( const char* name, unsigned size );
	
	quad_t parse_quad_name( const plus::string& name );
	
	void inscribe_quad_name( plus::var_string& out, quad_t q );
	
}

#endif

