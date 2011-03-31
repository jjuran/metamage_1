/*
	plus/string/concat.hh
	---------------------
*/

#ifndef PLUS_STRING_CONCAT_HH
#define PLUS_STRING_CONCAT_HH

// iota
#include "iota/string_traits.hh"

// plus
#include "plus/string.hh"


namespace plus
{
	
	string concat( const char*  a, string::size_type  a_size,
	               const char*  b, string::size_type  b_size );
	
	template < class A >
	string concat( const A&     a,
	               const char*  b, string::size_type  b_size )
	{
		return concat( iota::get_string_data( a ),
		               iota::get_string_size( a ),
		               b,
		               b_size );
	}
	
	template < class B >
	string concat( const char*  a, string::size_type a_size,
	               const B&     b )
	{
		return concat( a,
		               a_size,
		               iota::get_string_data( b ),
		               iota::get_string_size( b ) );
	}
	
	template < class A, class B >
	string concat( const A&  a,
	               const B&  b )
	{
		return concat( iota::get_string_data( a ),
		               iota::get_string_size( a ),
		               iota::get_string_data( b ),
		               iota::get_string_size( b ) );
	}
	
	string operator+( const string& a, const string& b );
	
	string operator+( const string& a, const char* b );
	
	string operator+( const char* a, const string& b );
	
}

#endif

