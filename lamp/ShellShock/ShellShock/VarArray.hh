// ===========
// VarArray.hh
// ===========

#ifndef SHELLSHOCK_VARARRAY_HH
#define SHELLSHOCK_VARARRAY_HH

// ShellShock
#include "ShellShock/StringArray.hh"


namespace ShellShock
{
	
	// Compare two strings for equality.
	// CompareStrings( a, b ) is equivalent to ( strcmp( a, b ) == 0 ).
	
	inline bool CompareStrings( const char* a, const char* b )
	{
		return std::lexicographical_compare( a, a + std::strlen( a ),
		                                     b, b + std::strlen( b ) );
	}
	
	// For input of "name" or "name=value", return a pointer to the byte following name.
	// For NULL, return NULL (identity).
	
	const char* EndOfVarName( const char* var );
	
	// True if the sequence (var, end) == (name, name + strlen(name)).
	// If var is NULL, it returns false.
	// end and name may be NULL if var is NULL.
	
	bool VarMatchesName( const char* var, const char* end, const char* name );
	
	// Allocates a new string of the form "name=value".
	// If value is NULL, the new string is just "name".
	// This string must be disposed with delete[].
	
	char* NewVar( const char* name, const char* value );
	
	// Disposes a string created with NewVar().
	
	void ReleaseVar( char* var );
	
	inline std::string MakeVar( const std::string& name, const char* value )
	{
		return value != NULL ? name + "=" + value : name;
	}
	
	class VarArray : public StringArray
	{
		public:
			VarArray()  {}
			VarArray( char* const* arrayPointer ) : StringArray( arrayPointer )  {}
			
			SVector::const_iterator Find( const char* name ) const;
			SVector::iterator Find( const char* name );
			
			void SetVar( const char* name, const char* value, bool overwrite );
	};
	
}

#endif

