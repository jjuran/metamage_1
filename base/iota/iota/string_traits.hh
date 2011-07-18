/*
	iota/string_traits.hh
	---------------------
*/

#ifndef IOTA_STRINGTRAITS_HH
#define IOTA_STRINGTRAITS_HH

// Standard C/C++
#include <cstring>

// iota
#include "iota/pascal_string.hh"


namespace iota
{
	
	// Overloads so Metrowerks C++ will inline them
	
	inline const char* get_string_c_str( const char* s )
	{
		return s;
	}
	
	inline const char* get_string_data( const char* s )
	{
		return s;
	}
	
	inline std::size_t get_string_size( const char* s )
	{
		return std::strlen( s );
	}
	
	
	inline const char* get_string_c_str( char* s )
	{
		return s;
	}
	
	inline const char* get_string_data( char* s )
	{
		return s;
	}
	
	inline std::size_t get_string_size( char* s )
	{
		return std::strlen( s );
	}
	
	
#if IOTA_HAS_PASCAL_STRINGS
	
	inline const char* get_string_data( const unsigned char* s )
	{
		return get_pascal_string_data( s );
	}
	
	inline const char* get_string_data( unsigned char* s )
	{
		return get_pascal_string_data( s );
	}
	
	inline std::size_t get_string_size( const unsigned char* s )
	{
		return get_pascal_string_size( s );
	}
	
	inline std::size_t get_string_size( unsigned char* s )
	{
		return get_pascal_string_size( s );
	}
	
#endif
	
	template < class String >
	inline const char*
	//
	get_string_c_str( const String& s )
	{
		return s.c_str();
	}
	
	template < class String >
	inline const char*
	//
	get_string_data( const String& s )
	{
		return s.data();
	}
	
	template < class String >
	inline std::size_t get_string_size( const String& s )
	{
		return s.size();
	}
	
}

#endif

