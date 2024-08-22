/*
	transcode.cc
	------------
*/

#include "callout/transcode.hh"

// transcodex
#include "transcode/8x_1bpp_to_8bpp.hh"


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

int32_t transcode_8x_1bpp_to_8bpp_callout( v68k::processor_state& s )
{
	const v68k::function_code_t data_space = s.data_space();
	
	uint32_t n_src = s.d(0);
	
	if ( n_src == 0 )
	{
		return rts;
	}
	
	const uint32_t n_dst = n_src * 8;
	
	const uint32_t src = s.a(0);
	const uint32_t dst = s.a(1);
	
	const uint8_t* p = s.translate( src, n_src, data_space, mem_read );
	
	if ( p == NULL )
	{
		goto fail;
	}
	
	uint8_t* q;
	
	q = s.translate( dst, n_dst, data_space, mem_write );
	
	if ( q == NULL )
	{
		goto fail;
	}
	
	uint8_t c0;
	uint8_t c1;
	
	c0 = s.d(1);
	c1 = s.d(2);
	
	transcode::_8x_1bpp_to_8bpp( p, q, n_src, c0, c1 );
	
	s.translate( dst, n_dst, data_space, mem_update );
	
	s.d(0) = 0;
	
	return rts;
	
fail:
	
	s.d(0) = -1;
	
	return rts;
}

}  // namespace callout
}  // namespace v68k
