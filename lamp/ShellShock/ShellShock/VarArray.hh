// ===========
// VarArray.hh
// ===========

#ifndef SHELLSHOCK_VARARRAY_HH
#define SHELLSHOCK_VARARRAY_HH

// Standard C++
#include <string>

// Standard C/C++
#include <cstring>


namespace ShellShock
{
	
	// C string 'less'
	
	inline bool CompareStrings( const char* a, const char* b )
	{
		return std::strcmp( a, b ) < 0;
	}
	
	// For input of "name" or "name=value", return a pointer to the byte following name.
	// For NULL, return NULL (identity).
	
	const char* EndOfVarName( const char* var );
	
	// True if the sequence (var, end) == (name, name + strlen(name)).
	// If var is NULL, it returns false.
	// end and name may be NULL if var is NULL.
	
	bool VarMatchesName( const char* var, const char* end, const char* name );
	
	inline std::string MakeVar( const std::string& name, const char* value )
	{
		return value != NULL ? name + "=" + value : name;
	}
	
}

#endif

