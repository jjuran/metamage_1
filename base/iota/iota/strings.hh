/*	===============
 *	iota/strings.hh
 *	===============
 */

#ifndef IOTA_STRINGS_HH
#define IOTA_STRINGS_HH

// Expands to a value equal to strlen( str ), but for string constants only,
// and in constant (zero) time instead of linear time.
#define STRLEN( str )  (sizeof "" str - 1)

// Expands to two comma-separated values -- the string constant and its length.
#define STR_LEN( str )  str, STRLEN( str )


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

