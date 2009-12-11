/*
	stringify.cc
	------------
*/

#include "Genie/FS/stringify.hh"

// iota
#include "iota/decimal.hh"

// plus
#include "plus/hexidecimal.hh"


namespace Genie
{
	
	std::string stringify_short::apply( short x, bool binary )
	{
		return binary ? std::string( (char*) &x, sizeof x )
		              : iota::inscribe_decimal( x );
	}
	
	std::string stringify_int::apply( int x, bool binary )
	{
		return binary ? std::string( (char*) &x, sizeof x )
		              : iota::inscribe_decimal( x );
	}
	
	std::string stringify_unsigned::apply( unsigned x, bool binary )
	{
		return binary ? std::string( (char*) &x, sizeof x )
		              : iota::inscribe_unsigned_decimal( x );
	}
	
	std::string stringify_16_bit_hex::apply( unsigned short x, bool binary )
	{
		return binary ? std::string( (char*) &x, sizeof x )
		              : plus::encode_16_bit_hex( x );
	}
	
	std::string stringify_32_bit_hex::apply( unsigned x, bool binary )
	{
		return binary ? std::string( (char*) &x, sizeof x )
		              : plus::encode_32_bit_hex( x );
	}
	
	std::string stringify_pascal_string::apply( const unsigned char* s, bool binary )
	{
		return std::string( (char*) &s[1], 0 + s[0] );
	}
	
}

