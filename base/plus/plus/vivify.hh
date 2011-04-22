/*
	vivify.hh
	---------
*/

#ifndef PLUS_VIVIFY_HH
#define PLUS_VIVIFY_HH

// iota
#include "iota/hexidecimal.hh"

// gear
#include "gear/parse_decimal.hh"


namespace plus
{
	
	struct vivify_char
	{
		typedef char result_type;
		
		static char apply( const char* begin, const char* end )
		{
			return begin != end ? *begin : '\0';
		}
	};
	
	struct vivify_bool
	{
		typedef bool result_type;
		
		static bool apply( const char* begin, const char* end )
		{
			return gear::parse_unsigned_decimal( begin ) != 0;
		}
	};
	
	template < class Int >
	struct vivify_int
	{
		typedef Int result_type;
		
		static Int apply( const char* begin, const char* end )
		{
			return Int( gear::parse_decimal( begin ) );
		}
	};
	
	template < class Int >
	struct vivify_unsigned
	{
		typedef Int result_type;
		
		static Int apply( const char* begin, const char* end )
		{
			return Int( gear::parse_unsigned_decimal( begin ) );
		}
	};
	
	struct vivify_8_bit_hex
	{
		static unsigned char apply( const char* begin, const char* end )
		{
			return iota::decode_8_bit_hex( begin );
		}
	};
	
	struct vivify_16_bit_hex
	{
		static unsigned short apply( const char* begin, const char* end )
		{
			return iota::decode_16_bit_hex( begin );
		}
	};
	
	struct vivify_32_bit_hex
	{
		static unsigned apply( const char* begin, const char* end )
		{
			return iota::decode_32_bit_hex( begin );
		}
	};
	
	
	template < int size > struct hex_vivifier;
	
	template <> struct hex_vivifier< 1 >
	{
		typedef vivify_8_bit_hex type;
	};
	
	template <> struct hex_vivifier< 2 >
	{
		typedef vivify_16_bit_hex type;
	};
	
	template <> struct hex_vivifier< 4 >
	{
		typedef vivify_32_bit_hex type;
	};
	
}

#endif

