// ===========
// VarArray.cc
// ===========

#include "ShellShock/VarArray.hh"

// Standard C++
#include <algorithm>


namespace ShellShock
{
	
	// For input of "name" or "name=value", return a pointer to the byte following name.
	// For NULL, return NULL (identity).
	
	const char* EndOfVarName( const char* var )
	{
		if ( var == NULL )
		{
			return NULL;
		}
		
		const char* end = std::strchr( var, '=' );
		
		if ( end == NULL )
		{
			end = std::strchr( var, '\0' );
		}
		
		return end;
	}
	
	// True if the sequence (var, end) == (name, name + strlen(name)).
	// If var is NULL, it returns false.
	// end and name may be NULL if var is NULL.
	
	bool VarMatchesName( const char* var, const char* end, const char* name )
	{
		return    var != NULL
		       && end - var == std::strlen( name )
		       && std::equal( var, end, name );
	}
	
}

