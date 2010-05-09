/*
	stringify.hh
	------------
	
	Copyright 2009, Joshua Juran
*/

#ifndef GENIE_FS_STRINGIFY_HH
#define GENIE_FS_STRINGIFY_HH

// plus
#include "plus/string.hh"


namespace Genie
{
	
	struct stringify_short
	{
		static plus::string apply( short x, bool binary );
	};
	
	struct stringify_int
	{
		static plus::string apply( int x, bool binary );
	};
	
	struct stringify_unsigned
	{
		static plus::string apply( unsigned x, bool binary );
	};
	
	struct stringify_unsigned_wide
	{
		static plus::string apply( unsigned long long x, bool binary );
	};
	
	struct stringify_16_bit_hex
	{
		static plus::string apply( unsigned short x, bool binary );
	};
	
	struct stringify_32_bit_hex
	{
		static plus::string apply( unsigned x, bool binary );
	};
	
	struct stringify_pointer
	{
		static plus::string apply( const void* x, bool binary )
		{
			return stringify_32_bit_hex::apply( (unsigned) x, binary );
		}
	};
	
	struct stringify_string
	{
		static plus::string apply( const char* s, bool binary )
		{
			return s;
		}
	};
	
	struct stringify_pascal_string
	{
		static plus::string apply( const unsigned char* s, bool binary );
	};
	
}

#endif

