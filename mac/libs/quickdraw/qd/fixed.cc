/*
	fixed.cc
	--------
*/

#include "qd/fixed.hh"


namespace quickdraw
{
	
	int32_t fix_mul( int32_t a, int32_t b )
	{
		const uint64_t product = (int64_t) a * b;
		
		int64_t rounded_product = product + 0x8000;
		
		if ( rounded_product < 0  &&  (int64_t) product >= 0 )
		{
			return 0x7FFFFFFF;
		}
		
		rounded_product >>= 16;
		
		if ( rounded_product != (int32_t) rounded_product )
		{
			const bool negative = (a < 0) != (b < 0);
			
			return negative ? 0x80000000 : 0x7FFFFFFF;
		}
		
		return rounded_product;
	}
	
	int32_t fix_ratio( int16_t numer, int16_t denom )
	{
		if ( denom == 1 )
		{
			return numer * 65536;
		}
		
		if ( denom == -1 )
		{
			if ( numer == -32768 )
			{
				return 0x7FFFFFFF;
			}
			
			return -numer * 65536;
		}
		
		if ( denom == 0 )
		{
			return numer < 0 ? 0x80000000 : 0x7FFFFFFF;
		}
		
		const int32_t quotient  = numer * 65536 / denom;
		const int32_t remainder = numer * 65536 % denom;
		
		return quotient + 2 * remainder / denom;
	}
	
	int32_t fix_div( int32_t dividend, int32_t divisor )
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
		
		int64_t dividend_64 = (uint64_t) dividend << 32;
		int64_t divisor_64  = (uint64_t) divisor  << 16;
		
		dividend_64 += divisor_64 / 2;
		
		return dividend_64 / divisor_64;
	}
	
}
