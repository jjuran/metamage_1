/*
	argv.cc
	-------
*/

#include "relix/task/argv.hh"

// Standard C
#include <string.h>

// Standard C++
#include <algorithm>

// more-libc
#include "more/string.h"

// debug
#include "debug/assert.hh"


namespace relix
{
	
	static inline
	unsigned sizeof_argv_strings( char const* const* argv )
	{
		unsigned size = 0;
		
		// Check for NULL environ
		
		if ( argv != NULL )
		{
			while ( const char* p = *argv++ )
			{
				size += strlen( p ) + 1;  // include trailing NUL
			}
		}
		
		return size;
	}
	
	static
	void flatten_argv( plus::var_string& result, char const *const *argv )
	{
		unsigned size = sizeof_argv_strings( argv );
		
		char* q = result.reset( size );
		
		// Check for NULL environ
		
		if ( argv != NULL )
		{
			while ( *argv )
			{
				const char* p = *argv++;
				
				unsigned n = strlen( p ) + 1;  // include trailing NUL
				
				q = (char*) mempcpy( q, p, n );
			}
		}
	}
	
	static void assign_unflattened_argv( vxo::UPtrVec< char >& result, plus::var_string& flat )
	{
		result.clear();
		
		char* begin = &*flat.begin();
		char* end   = &*flat.end();
		
		while ( begin < end )
		{
			char* null = std::find( begin, end, '\0' );
			
			ASSERT( null != end );
			
			result.push_back( begin );
			
			begin = null + 1;
		}
		
		result.push_back( NULL );
	}
	
	
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
		flatten_argv( its_string, args );
		
		assign_unflattened_argv( its_vector, its_string );
		
		return *this;
	}
	
}
