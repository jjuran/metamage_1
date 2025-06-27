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
		
		char const* const* argp = args;
		
		// Check for NULL environ
		
		if ( argp != NULL )
		{
			while ( const char* p = *argp++ )
			{
				size += strlen( p ) + 1;  // include trailing NUL
			}
		}
		
		char* q = its_string.reset( size );
		
		its_vector.clear();
		
		// Check for NULL environ
		
		if ( args != NULL )
		{
			while ( const char* p = *args++ )
			{
				its_vector.push_back( q );
				
				unsigned n = strlen( p ) + 1;  // include trailing NUL
				
				q = (char*) mempcpy( q, p, n );
			}
		}
		
		its_vector.push_back( NULL );
		
		return *this;
	}
	
}
