/*
	macros.hh
	---------
*/

#ifndef V68K_MACROS_HH
#define V68K_MACROS_HH

// C99
#include <stdint.h>

// v68k
#include "v68k/instruction.hh"


namespace v68k
{
	
	inline int byte_count( op_size_t size )
	{
		return 1 << (size - 1);
	}
	
	inline int bit_count( op_size_t size )
	{
		return byte_count( size ) * 8;
	}
	
	uint32_t zero_extend( uint32_t data, op_size_t size );
	
	int32_t sign_extend( int32_t data, op_size_t size );
	
	uint32_t update( uint32_t dest, uint32_t src, op_size_t size );
	
}

#endif
