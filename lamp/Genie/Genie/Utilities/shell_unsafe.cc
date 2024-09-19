/*
	shell_unsafe.cc
	---------------
*/

#include "Genie/Utilities/shell_unsafe.hh"

// Standard C
#include <string.h>


namespace Genie
{

static inline
bool shell_unsafe( char c )
{
	return strchr( " !#$&*()[]{}\\;'\"<>?`~", c );
}

long shell_unsafe_index( const plus::string& s )
{
	const size_t n = s.size();
	
	const char* data = s.data();
	
	for ( size_t i = 0;  i < n;  ++i )
	{
		if ( shell_unsafe( data[ i ] ) )
		{
			return i;
		}
	}
	
	return -1;
}

plus::string safe_prefix( const plus::string& s )
{
	ssize_t i = shell_unsafe_index( s );
	
	if ( i >= 0 )
	{
		return s.substr( 0, i );
	}
	
	return s;
}

}
