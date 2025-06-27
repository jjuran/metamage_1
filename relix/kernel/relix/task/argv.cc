/*
	argv.cc
	-------
*/

#include "relix/task/argv.hh"

// Standard C
#include <string.h>

// more-libc
#include "more/string.h"


namespace relix
{
	
	argv::argv()
	{
		its_vector.push_back( NULL );
	}
	
	void argv::swap( argv& that )
	{
		its_string.swap( that.its_string );
		its_vector.swap( that.its_vector );
	}
	
	argv& argv::assign( const char *const *args )
	{
		unsigned size = 0;
		unsigned n    = 1;
		
		char const* const* argp = args;
		
		// Check for NULL environ
		
		if ( argp != NULL )
		{
			while ( const char* p = *argp++ )
			{
				size += strlen( p ) + 1;  // include trailing NUL
				
				++n;
			}
		}
		
		char* q = its_string.reset( size );
		
		its_vector.clear();
		
		vxo::anyptr_t* r = its_vector.expand_by( n );
		
		// Check for NULL environ
		
		if ( args != NULL )
		{
			while ( const char* p = *args++ )
			{
				*r++ = q;
				
				unsigned n = strlen( p ) + 1;  // include trailing NUL
				
				q = (char*) mempcpy( q, p, n );
			}
		}
		
		*r++ = NULL;
		
		return *this;
	}
	
}
