/*
	rgn.hh
	------
*/

#ifndef RGN_RGN_HH
#define RGN_RGN_HH

// Standard
#include <stdint.h>

// iota
#include "iota/endian.hh"


namespace rgn
{
	
	typedef iota::big_endian_conversion< uint16_t >::type U_16;
	
	struct rect
	{
		U_16 top;
		U_16 left;
		U_16 bottom;
		U_16 right;
	};
	
	struct header
	{
		U_16 size;
		rect bbox;
	};
	
	bool valid( const header& h, uint32_t size );
	
}

#endif
