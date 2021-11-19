/*
	plus/mac_utf8.hh
	----------------
*/

#ifndef PLUS_MACUTF8_HH
#define PLUS_MACUTF8_HH

// iota
#include "iota/string_traits.hh"

// plus
#include "plus/string.hh"


namespace plus
{
	
	string::size_type sizeof_utf8_from_mac( const string& mac  );
	string::size_type sizeof_mac_from_utf8( const string& utf8 );
	
	string utf8_from_mac( const char* p, string::size_type n );
	string mac_from_utf8( const char* p, string::size_type n );
	
	string utf8_from_mac( const string& mac  );
	string mac_from_utf8( const string& utf8 );
	
	template < class String >
	inline string utf8_from_mac( const String& mac )
	{
		using iota::get_string_data;
		using iota::get_string_size;
		
		return utf8_from_mac( get_string_data( mac ),
		                      get_string_size( mac ) );
	}
	
	template < class String >
	inline string mac_from_utf8( const String& utf8 )
	{
		using iota::get_string_data;
		using iota::get_string_size;
		
		return mac_from_utf8( get_string_data( utf8 ),
		                      get_string_size( utf8 ) );
	}
	
}

#endif
