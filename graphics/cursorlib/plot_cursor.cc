/*
	plot_cursor.cc
	--------------
*/

#include "plot_cursor.hh"

// iota
#include "iota/endian.hh"


void plot_cursor( const uint16_t*  crsr_face,
                  const uint16_t*  crsr_mask,
                  uint8_t*         addr,
                  short            shift,
                  short            h_trim,
                  short            v_skip,
                  short            n,
                  short            stride )
{
	const uint16_t* p_face = crsr_face + v_skip;
	const uint16_t* p_mask = crsr_mask + v_skip;
	
	for ( short i = 0;  i < n;  ++i )
	{
		uint16_t face = iota::u16_from_big( *p_face++ );
		uint16_t mask = iota::u16_from_big( *p_mask++ );
		
		uint16_t* q = (uint16_t*) addr;
		
		if ( h_trim >= 0 )
		{
			*q   &= iota::big_u16( ~(mask >> shift) );
			*q++ ^= iota::big_u16(   face >> shift  );
		}
		
		if ( shift  &&  h_trim <= 0 )
		{
			*q &= iota::big_u16( ~(mask << (16 - shift)) );
			*q ^= iota::big_u16(   face << (16 - shift)  );
		}
		
		addr += stride;
	}
}
