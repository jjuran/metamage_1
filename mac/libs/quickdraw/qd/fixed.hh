/*
	fixed.hh
	--------
*/

#ifndef QUICKDRAW_FIXED_HH
#define QUICKDRAW_FIXED_HH

// Standard C
#include <stdint.h>


namespace quickdraw
{
	
	int32_t fix_mul( int32_t a, int32_t b );
	
	int32_t fix_ratio( int16_t numer, int16_t denom );
	
	inline
	int16_t fix_round( int32_t x )
	{
		const uint32_t unrounded = x;
		
		const int32_t rounded = unrounded + 0x8000;
		
		if ( rounded < 0  &&  x >= 0 )
		{
			return 0x7FFF;
		}
		
		return rounded >> 16;
	}
	
	int32_t fix_div( int32_t dividend, int32_t divisor );
	
}

#endif
