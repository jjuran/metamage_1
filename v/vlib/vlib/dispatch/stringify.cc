/*
	stringify.cc
	------------
*/

#include "vlib/dispatch/stringify.hh"

// Standard C
#include <string.h>

// more-libc
#include "more/string.h"

#include "vlib/string-utils.hh"


namespace vlib
{
	
	size_t get_stringified_size( const stringify& methods, const Value& v )
	{
		if ( methods.size )
		{
			return methods.size( v );
		}
		
		if ( methods.data )
		{
			return strlen( methods.data( v ) );
		}
		
		return 0;  // FIXME:  Throw an exception?
	}
	
	char* copy_stringified( char* p, const stringify& methods, const Value& v )
	{
		if ( methods.copy )
		{
			return methods.copy( p, v );
		}
		
		const size_t size = get_stringified_size( methods, v );
		
		if ( size != 0 )
		{
			const char* data = methods.data( v );
			
			p = (char*) mempcpy( p, data, size );
		}
		
		return p;
	}
	
}
