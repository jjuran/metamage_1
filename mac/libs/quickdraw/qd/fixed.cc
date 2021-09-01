/*
	fixed.cc
	--------
*/

#include "qd/fixed.hh"


namespace quickdraw
{
	
	long fix_mul( long a, long b )
	{
		const unsigned long long product = (long long) a * b;
		
		long long rounded_product = product + 0x8000;
		
		if ( rounded_product < 0  &&  (long long) product >= 0 )
		{
			return 0x7FFFFFFF;
		}
		
		rounded_product >>= 16;
		
		if ( rounded_product != (long) rounded_product )
		{
			const bool negative = (a < 0) != (b < 0);
			
			return negative ? 0x80000000 : 0x7FFFFFFF;
		}
		
		return rounded_product;
	}
	
	long fix_ratio( short numer, short denom )
	{
		if ( denom == 1 )
		{
			return numer << 16;
		}
		
		if ( denom == -1 )
		{
			if ( numer == -32768 )
			{
				return 0x7FFFFFFF;
			}
			
			return -numer << 16;
		}
		
		if ( denom == 0 )
		{
			return numer < 0 ? 0x80000000 : 0x7FFFFFFF;
		}
		
		const long quotient  = (numer << 16) / denom;
		const long remainder = (numer << 16) % denom;
		
		return quotient + 2 * remainder / denom;
	}
	
	long fix_div( long dividend, long divisor )
	{
		if ( divisor == 0x10000 )
		{
			return dividend;
		}
		
		if ( divisor == -0x10000 )
		{
			if ( dividend == -2147483648 )
			{
				return 0x7FFFFFFF;
			}
			
			return -dividend;
		}
		
		if ( divisor == 0 )
		{
			return dividend < 0 ? 0x80000000 : 0x7FFFFFFF;
		}
		
		long long dividend_64 = (unsigned long long) dividend << 32;
		long long divisor_64  = (unsigned long long) divisor  << 16;
		
		dividend_64 += divisor_64 / 2;
		
		return dividend_64 / divisor_64;
	}
	
}
