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
	
	plus::string stringify_short::apply( short x, bool binary )
	{
		return binary ? plus::string( (char*) &x, sizeof x )
		              : iota::inscribe_decimal( x );
	}
	
	plus::string stringify_int::apply( int x, bool binary )
	{
		return binary ? plus::string( (char*) &x, sizeof x )
		              : iota::inscribe_decimal( x );
	}
	
	plus::string stringify_unsigned::apply( unsigned x, bool binary )
	{
		return binary ? plus::string( (char*) &x, sizeof x )
		              : iota::inscribe_unsigned_decimal( x );
	}
	
	plus::string stringify_unsigned_wide::apply( unsigned long long x, bool binary )
	{
		return binary ? plus::string( (char*) &x, sizeof x )
		              : iota::inscribe_unsigned_wide_decimal( x );
	}
	
	plus::string stringify_16_bit_hex::apply( unsigned short x, bool binary )
	{
		return binary ? plus::string( (char*) &x, sizeof x )
		              : plus::encode_16_bit_hex( x );
	}
	
	plus::string stringify_32_bit_hex::apply( unsigned x, bool binary )
	{
		return binary ? plus::string( (char*) &x, sizeof x )
		              : plus::encode_32_bit_hex( x );
	}
	
	plus::string stringify_pascal_string::apply( const unsigned char* s, bool binary )
	{
		return plus::string( (char*) &s[1], 0 + s[0] );
	}
	
}

