/*
	stringify.hh
	------------
	
	Copyright 2009, Joshua Juran
*/

#ifndef GENIE_FS_STRINGIFY_HH
#define GENIE_FS_STRINGIFY_HH

// Standard C++
#include <string>


namespace Genie
{
	
	struct stringify_short
	{
		static std::string apply( short x, bool binary );
	};
	
	struct stringify_int
	{
		static std::string apply( int x, bool binary );
	};
	
	struct stringify_unsigned
	{
		static std::string apply( unsigned x, bool binary );
	};
	
	struct stringify_unsigned_wide
	{
		static std::string apply( unsigned long long x, bool binary );
	};
	
	struct stringify_16_bit_hex
	{
		static std::string apply( unsigned short x, bool binary );
	};
	
	struct stringify_32_bit_hex
	{
		static std::string apply( unsigned x, bool binary );
	};
	
	struct stringify_pointer
	{
		static std::string apply( const void* x, bool binary )
		{
			return stringify_32_bit_hex::apply( (unsigned) x, binary );
		}
	};
	
	struct stringify_string
	{
		static std::string apply( const char* s, bool binary )
		{
			return s;
		}
	};
	
	struct stringify_pascal_string
	{
		static std::string apply( const unsigned char* s, bool binary );
	};
	
}

#endif

