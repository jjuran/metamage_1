/*
	iota/convert_string.hh
	----------------------
	
	Copyright 2010, Joshua Juran
*/

#ifndef IOTA_CONVERTSTRING_HH
#define IOTA_CONVERTSTRING_HH

// iota
#include "iota/string_traits.hh"


namespace iota
{
	
	template < class Output, class Input >
	struct string_converter
	{
		typedef Output result_type;
		
		static Output apply( const Input& input )
		{
			return Output( get_string_data( input ), get_string_size( input ) );
		}
	};
	
	template < class String >
	struct string_converter< String, String >
	{
		typedef const String& result_type;
		
		static const String& apply( const String& input )
		{
			return input;
		}
	};
	
	
	template < class Output, class Input >
	inline typename string_converter< Output, Input >::result_type
	//
	convert_string( const Input& input )
	{
		return string_converter< Output, Input >::apply( input );
	}
	
}

#endif

