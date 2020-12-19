/*
	plus/argv.cc
	------------
*/

#include "plus/argv.hh"

// Standard C++
#include <algorithm>

// Standard C/C++
#include <cstring>

// debug
#include "debug/assert.hh"


namespace plus
{
	
	static plus::string flattened_argv( char const *const *argv )
	{
		plus::var_string result;
		
		// Check for NULL environ
		
		if ( argv != NULL )
		{
			while ( *argv )
			{
				const char* p = *argv++;
				
				result.append( p, std::strlen( p ) + 1 );  // include trailing NUL
			}
		}
		
		return result.move();
	}
	
	static void assign_unflattened_argv( std::vector< char* >& result, plus::var_string& flat )
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
	
	argv::argv( const plus::string& s ) : its_string( s )
	{
		assign_unflattened_argv( its_vector, its_string );
	}
	
	argv::argv( const char *const *args ) : its_string( flattened_argv( args ) )
	{
		assign_unflattened_argv( its_vector, its_string );
	}
	
	argv::argv( const argv& that ) : its_string( that.its_string )
	{
		assign_unflattened_argv( its_vector, its_string );
	}
	
	argv& argv::operator=( const argv& that )
	{
		// The self-assignment case is inefficient, but harmless
		
		return assign( that.its_string );
	}
	
	void argv::swap( argv& that )
	{
		its_string.swap( that.its_string );
		its_vector.swap( that.its_vector );
	}
	
	argv& argv::assign( const plus::string& s )
	{
		its_string = s;
		
		assign_unflattened_argv( its_vector, its_string );
		
		return *this;
	}
	
	argv& argv::assign( const char *const *args )
	{
		return assign( flattened_argv( args ) );
	}
	
}
