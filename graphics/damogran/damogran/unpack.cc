/*
	unpack.cc
	---------
*/

#include "damogran/unpack.hh"

// Standard C
#include <stddef.h>
#ifndef __MC68K__
#include <string.h>
#endif


namespace damogran
{

#ifdef __MC68K__

static asm
void memcpy( uint8_t* dst : __A1, const uint8_t* src : __A0, int n : __D0 )
{
	LSR.L    #1,D0
	SUBQ.L   #1,D0
	
loop:
	MOVE.W   (A0)+,(A1)+
	DBRA.S   D0,loop
	
	RTS
}

static asm
void memset( uint8_t* dst : __A1, uint8_t fill : __D1, int n : __D0 )
{
	MOVE.B   D1,D2
	LSL.W    #8,D2
	MOVE.B   D1,D2
	
	LSR.L    #1,D0
	SUBQ.L   #1,D0
	
loop:
	MOVE.W   D2,(A1)+
	DBRA.S   D0,loop
	
	RTS
}

#endif  // #ifdef __MC68K__

typedef signed char int8_t;

long unpack_preflight( const uint8_t* src, const uint8_t* end )
{
	long len = 0;
	
	do
	{
		if ( src + 2 > end )  return -1;
		
		const int8_t c0 = *src++;
		const int8_t c1 = *src++;
		
		if ( c0 == 0 )
		{
			if ( c1 == 0 )
			{
				continue;
			}
			
			if ( c1 < 0 )
			{
				const size_t n = 2 * (uint8_t) -c1;
				
				len += n;
				src += n;
				
				continue;
			}
			
			// c1 > 0
			
			if ( src + 2 > end )  return -1;
			
			const uint8_t c2 = *src++;
			const uint8_t c3 = *src++;
			
			const size_t n = 2 * (c1 * 256 + c2 + 1);
			
			len += n;
		}
		else if ( c0 > 0 )
		{
			const size_t n = 2 * (c0 + 1);
			
			len += n;
		}
		else
		{
			return -1;  // (c0 < 0) is undefined
		}
	}
	while ( src < end );
	
	return src > end ? -1 : len;
}

const uint8_t* unpack( const uint8_t* src, uint8_t* dst, uint8_t* end )
{
	do
	{
		const int8_t c0 = *src++;
		const int8_t c1 = *src++;
		
		if ( c0 == 0 )
		{
			if ( c1 == 0 )
			{
				continue;
			}
			
			if ( c1 < 0 )
			{
				const size_t n = 2 * (uint8_t) -c1;
				
				memcpy( dst, src, n );
				
				src += n;
				dst += n;
				
				continue;
			}
			
			// c1 > 0
			
			const uint8_t c2 = *src++;
			const uint8_t c3 = *src++;
			
			const size_t n = 2 * (c1 * 256 + c2 + 1);
			
			memset( dst, c3, n );
			
			dst += n;
		}
		else if ( c0 > 0 )
		{
			const size_t n = 2 * (c0 + 1);
			
			memset( dst, c1, n );
			
			dst += n;
		}
	}
	while ( dst < end );
	
	return src;
}

}  // namespace damogran
