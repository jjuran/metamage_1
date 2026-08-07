/*
	bits.cc
	-------
*/

#include "bits/bits.hh"


namespace bits
{

bool valid( const header& h, uint32_t size )
{
	if ( size < sizeof h )
	{
		return false;
	}
	
	const int16_t stride = h.stride;
	const int16_t height = h.height;
	const int16_t width  = h.width;
	
	const uint32_t data_size = stride * height;
	
	return h.magic == bits::magic  &&
	       stride > 0              &&
	       height > 0              &&
	       width  > 0              &&
	       stride * 8 >= width     &&
	       (stride & 0x1) == 0     &&
	       size >= data_size;
	
}

}
