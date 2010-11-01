/*
	stringify.hh
	------------
*/

#ifndef PLUS_STRINGIFY_HH
#define PLUS_STRINGIFY_HH

// Standard C/C++
#include <cstring>

// iota
#include "iota/decimal.hh"

// plus
#include "plus/hexidecimal.hh"
#include "plus/var_string.hh"


namespace plus
{
	
	struct stringify_bool
	{
		static void apply( var_string& out, bool x )
		{
			out += '0' + x;
		}
	};
	
	struct stringify_int
	{
		static void apply( var_string& out, int x )
		{
			out += iota::inscribe_decimal( x );
		}
	};
	
	struct stringify_unsigned
	{
		static void apply( var_string& out, unsigned x )
		{
			out += iota::inscribe_unsigned_decimal( x );
		}
	};
	
	struct stringify_unsigned_wide
	{
		static void apply( var_string& out, unsigned long long x )
		{
			out += iota::inscribe_unsigned_wide_decimal( x );
		}
	};
	
	struct stringify_16_bit_hex
	{
		static void apply( var_string& out, unsigned short x )
		{
			encode_16_bit_hex( out, x );
		}
	};
	
	struct stringify_32_bit_hex
	{
		static void apply( var_string& out, unsigned x )
		{
			encode_32_bit_hex( out, x );
		}
	};
	
	struct stringify_pointer
	{
		static void apply( var_string& out, const void* x )
		{
			out += encode_32_bit_hex( (unsigned) x );
		}
	};
	
	struct stringify_c_string
	{
		static const char* get_data( const char* s )
		{
			return s;
		}
		
		static std::size_t get_size( const char* s )
		{
			return std::strlen( s );
		}
		
		static void apply( var_string& out, const char* s )
		{
			out += s;
		}
	};
	
	struct stringify_pascal_string
	{
		static const char* get_data( const unsigned char* s )
		{
			return (const char*) &s[ 1 ];  // reinterpret_cast
		}
		
		static std::size_t get_size( const unsigned char* s )
		{
			return s[ 0 ];
		}
		
		static void apply( var_string& out, const unsigned char* s )
		{
			out.append( get_data( s ), get_size( s ) );
		}
	};
	
	
	template < int size > struct hex_stringifier;
	
	template <> struct hex_stringifier< 2 >
	{
		typedef stringify_16_bit_hex type;
	};
	
	template <> struct hex_stringifier< 4 >
	{
		typedef stringify_32_bit_hex type;
	};
	
}

#endif

