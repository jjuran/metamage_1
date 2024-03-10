/*
	codec.cc
	--------
*/

#include "callout/codec.hh"

// Standard C
#include <errno.h>

// MACElib
#include "MACE/decode.hh"


#ifdef __MWERKS__
#pragma exceptions off
#endif


#ifndef NULL
#define NULL  0L
#endif


namespace v68k    {
namespace callout {

enum
{
	rts = 0x4E75,
};

int32_t decode_MACE3_callout( v68k::processor_state& s )
{
	const v68k::function_code_t data_space = s.data_space();
	
	const uint32_t src = s.a(0);
	const uint32_t dst = s.a(1);
	const uint32_t length = s.d(0);
	
	const uint8_t* p = s.translate( src, length, data_space, mem_read );
	
	if ( p == NULL )
	{
		s.d(0) = (uint32_t) -1;
		s.d(1) = EFAULT;
		
		return rts;
	}
	
	uint8_t* q = s.translate( dst, length * 3, data_space, mem_write );
	
	if ( q == NULL )
	{
		s.d(0) = (uint32_t) -1;
		s.d(1) = EFAULT;
		
		return rts;
	}
	
	MACE::decode_n_MACE3_to_u8( q, p, length );
	
	s.d(0)  = length;
	s.a(0) += length;
	s.a(1) += length * 3;
	
	s.translate( dst, length * 3, data_space, mem_update );
	
	return rts;
}

}  // namespace callout
}  // namespace v68k
