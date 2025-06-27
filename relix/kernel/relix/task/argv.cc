/*
	argv.cc
	-------
*/

#include "relix/task/argv.hh"

// Standard C
#include <string.h>

// Standard C++
#include <algorithm>

// debug
#include "debug/assert.hh"


namespace relix
{
	
	static
	void flatten_argv( plus::var_string& result, char const *const *argv )
	{
		result.clear();
		
		// Check for NULL environ
		
		if ( argv != NULL )
		{
			while ( *argv )
			{
				const char* p = *argv++;
				
				result.append( p, strlen( p ) + 1 );  // include trailing NUL
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
