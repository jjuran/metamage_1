/*
	macros.cc
	---------
*/

#include "v68k/macros.hh"


#ifdef __MWERKS__
#pragma exceptions off
#endif


namespace v68k
{
	
	uint32_t zero_extend( uint32_t data, op_size_t size )
	{
		switch ( size )
		{
			case byte_sized:  return uint8_t ( data );
			case word_sized:  return uint16_t( data );
		//	case long_sized:  return           data;
			default:
				break;
		}
		
		return data;
	}
	
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
	
	uint32_t update( uint32_t dest, uint32_t src, op_size_t size )
	{
		switch ( size )
		{
			default:
			case long_sized:  return src;
			case byte_sized:  return (dest & 0xFFFFFF00) | uint8_t ( src );
			case word_sized:  return (dest & 0xFFFF0000) | uint16_t( src );
		}
		
	}
	
}
