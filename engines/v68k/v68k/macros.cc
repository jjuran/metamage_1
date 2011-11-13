/*
	macros.cc
	---------
*/

#include "v68k/macros.hh"


namespace v68k
{
	
	int32_t sign_extend( int32_t data, op_size_t size )
	{
		switch ( size )
		{
			case byte_sized:  data = int16_t( int8_t ( data ) );  // fall through
			case word_sized:  data = int32_t( int16_t( data ) );
			case long_sized:
			default:
				break;
		}
		
		return data;
	}
	
}

