/*
	printable.cc
	------------
*/

#include "plus/printable.hh"

// Standard C
#include <stdint.h>

// gear
#include "gear/hexadecimal.hh"
#include "gear/inscribe_decimal.hh"


namespace plus
{
	
	string printable( const void* x )
	{
		string result;
		
		char* p = result.reset( 2 + 2 * sizeof x );  // "0x" ...
		
		*p++ = '0';
		*p++ = 'x';
		
		if ( sizeof x > 4 )
		{
			const unsigned upper = (uintptr_t) x >> (sizeof x * 8 - 32);
			
			gear::encode_32_bit_hex( upper, p );
			p += 8;
		}
		
		const unsigned lower = (uintptr_t) x;
		
		gear::encode_32_bit_hex( lower, p );
		
		return result;
	}
	
	string printable( unsigned long long x )
	{
		const unsigned n = gear::magnitude< 10 >( x );
		
		string result;
		
		char* p = result.reset( n );
		
		gear::fill_unsigned< 10 >( x, p, n );
		
		return result;
	}
	
	string printable( long long x )
	{
		const long long nadir = 1LL << (sizeof x * 8 - 1);
		
		const bool negative = x < 0;
		
		if ( negative  &&  x != nadir )
		{
			x = -x;
		}
		
		unsigned long long ux = x;
		
		const unsigned n = gear::magnitude< 10 >( ux );
		
		string result;
		
		char* p = result.reset( negative + n );
		
		if ( negative )
		{
			*p++ = '-';
		}
		
		gear::fill_unsigned< 10 >( ux, p, n );
		
		return result;
	}
	
}
