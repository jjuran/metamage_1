/*
	bits.hh
	-------
*/

#ifndef BITS_BITS_HH
#define BITS_BITS_HH

// Standard
#include <stdint.h>

// iota
#include "iota/endian.hh"


namespace bits
{
	
	typedef iota::big_endian_conversion< uint16_t >::type U_16;
	
	const uint16_t magic = iota::big_u16( 0xB175 );
	
	struct header
	{
		uint16_t magic;
		
		U_16 stride;
		U_16 height;
		U_16 width;
	};
	
	bool valid( const header& h, uint32_t size );
	
}

#endif
