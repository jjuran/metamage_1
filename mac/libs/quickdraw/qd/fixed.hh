/*
	fixed.hh
	--------
*/

#ifndef QUICKDRAW_FIXED_HH
#define QUICKDRAW_FIXED_HH

namespace quickdraw
{
	
	long fix_mul( long a, long b );
	
	long fix_ratio( short numer, short denom );
	
	inline
	short fix_round( long x )
	{
		const unsigned long unrounded = x;
		
		const long rounded = unrounded + 0x8000;
		
		if ( rounded < 0  &&  x >= 0 )
		{
			return 0x7FFF;
		}
		
		return rounded >> 16;
	}
	
	long fix_div( long dividend, long divisor );
	
}

#endif
