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
	
	string utf8_from_mac( const char* p, string::size_type n );
	string mac_from_utf8( const char* p, string::size_type n );
	
	string utf8_from_mac( const string& mac  );
	string mac_from_utf8( const string& utf8 );
	
	template < class String >
	inline string utf8_from_mac( const String& mac )
	{
		return utf8_from_mac( iota::string_data< String >::get( mac ),
		                      iota::string_size< String >::get( mac ) );
	}
	
	template < class String >
	inline string mac_from_utf8( const String& utf8 )
	{
		return mac_from_utf8( iota::string_data< String >::get( utf8 ),
		                      iota::string_size< String >::get( utf8 ) );
	}
	
}

#endif

