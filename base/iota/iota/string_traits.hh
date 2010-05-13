/*
	iota/string_traits.hh
	---------------------
	
	Copyright 2010, Joshua Juran
*/

#ifndef IOTA_STRINGTRAITS_HH
#define IOTA_STRINGTRAITS_HH


namespace iota
{
	
	template < class String >
	struct string_c_str
	{
		static const char* get( const String& s )
		{
			return s.c_str();
		}
	};
	
	template <>
	struct string_c_str< const char* >
	{
		static const char* get( const char* s )
		{
			return s;
		}
	};
	
	template <>
	struct string_c_str< char* > : string_c_str< const char* >
	{
	};
	
	template < unsigned n >
	struct string_c_str< const char[n] > : string_c_str< const char* >
	{
	};
	
#ifndef __MWERKS__
	
	template < unsigned n >
	struct string_c_str< char[n] > : string_c_str< const char* >
	{
	};
	
#endif
	
	
	template < class String >
	inline const char* get_string_c_str( const String& s )
	{
		return string_c_str< String >::get( s );
	}
	
}

#endif

