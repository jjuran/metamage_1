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
	struct string_const_pointer
	{
		typedef typename String::const_pointer type;
	};
	
	template <>
	struct string_const_pointer< const char* >
	{
		typedef const char* type;
	};
	
	template <>
	struct string_const_pointer< char* > : string_const_pointer< const char* >
	{
	};
	
	template < unsigned n >
	struct string_const_pointer< const char[n] > : string_const_pointer< const char* >
	{
	};
	
#ifndef __MWERKS__
	
	template < unsigned n >
	struct string_const_pointer< char[n] > : string_const_pointer< const char* >
	{
	};
	
#endif
	
	
	template < class String >
	struct string_c_str
	{
		static typename String::const_pointer get( const String& s )
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
	struct string_data
	{
		static typename String::const_pointer get( const String& s )
		{
			return s.data();
		}
	};
	
	template <>
	struct string_data< const char* > : string_c_str< const char* >
	{
	};
	
	template <>
	struct string_data< char* > : string_c_str< const char* >
	{
	};
	
	template < unsigned n >
	struct string_data< const char[n] > : string_c_str< const char* >
	{
	};
	
#ifndef __MWERKS__
	
	template < unsigned n >
	struct string_data< char[n] > : string_c_str< const char* >
	{
	};
	
#endif
	
	
	template < class String >
	struct string_size
	{
		static typename String::size_type get( const String& s )
		{
			return s.length();
		}
	};
	
	template <>
	struct string_size< const char* >
	{
		static unsigned long get( const char* );
	};
	
	template <>
	struct string_size< char* > : string_size< const char* >
	{
	};
	
	template < unsigned n >
	struct string_size< const char[n] > : string_size< const char* >
	{
	};
	
#ifndef __MWERKS__
	
	template < unsigned n >
	struct string_size< char[n] > : string_size< const char* >
	{
	};
	
#endif
	
	
#if defined( __MACOS__ )  ||  defined( __APPLE__ )
	
	template <>
	struct string_const_pointer< const unsigned char* > : string_const_pointer< const char* >
	{
	};
	
	template <>
	struct string_const_pointer< unsigned char* > : string_const_pointer< const char* >
	{
	};
	
	template < unsigned n >
	struct string_const_pointer< const unsigned char[n] > : string_const_pointer< const char* >
	{
	};
	
#ifndef __MWERKS__
	
	template < unsigned n >
	struct string_const_pointer< unsigned char[n] > : string_const_pointer< const char* >
	{
	};
	
#endif
	
	template <>
	struct string_data< const unsigned char* >
	{
		static const char* get( const unsigned char* s )
		{
			return (const char*) s + 1;
		}
	};
	
	template <>
	struct string_data< unsigned char* > : string_data< const unsigned char* >
	{
	};
	
	template < unsigned n >
	struct string_data< const unsigned char[n] > : string_data< const unsigned char* >
	{
	};
	
#ifndef __MWERKS__
	
	template < unsigned n >
	struct string_data< unsigned char[n] > : string_data< const unsigned char* >
	{
	};
	
#endif
	
	
	template <>
	struct string_size< const unsigned char* >
	{
		static unsigned long get( const unsigned char* s )
		{
			return s[0];
		}
	};
	
	template <>
	struct string_size< unsigned char* > : string_size< const unsigned char* >
	{
	};
	
	template < unsigned n >
	struct string_size< const unsigned char[n] > : string_size< const unsigned char* >
	{
	};
	
#ifndef __MWERKS__
	
	template < unsigned n >
	struct string_size< unsigned char[n] > : string_size< const unsigned char* >
	{
	};
	
#endif
	
#endif
	
	template < class String >
	inline typename string_const_pointer< String >::type
	//
	get_string_c_str( const String& s )
	{
		return string_c_str< String >::get( s );
	}
	
	template < class String >
	inline typename string_const_pointer< String >::type
	//
	get_string_data( const String& s )
	{
		return string_data< String >::get( s );
	}
	
	template < class String >
	inline unsigned long get_string_size( const String& s )
	{
		return string_size< String >::get( s );
	}
	
}

#endif

