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

#ifdef __MC68K__

	#define _a0  : __A0
	#define _a1  : __A1

asm
const uint8_t* unpack( const uint8_t* src _a0, uint8_t* dst _a1, uint8_t* end )
{
loop_top:
	MOVEQ.L  #0,D0
	MOVE.B   (A0)+,D2  // c0
	MOVE.B   (A0)+,D0  // c1
	
	TST.B    D2
	BNE.S    c0_nonzero
	
	TST.B    D0
	BEQ.S    loop_bottom
	BPL.S    c0_zero_c1_positive
	
	NEG.B    D0
	
// memcpy
	LSR.W    #1,D0
	BCC.S    memcpy_even_4x
	MOVE.W   (A0)+,(A1)+
memcpy_even_4x:
	
	SUBQ.W   #1,D0
	BMI.S    loop_bottom
	
memcpy_loop:
	MOVE.L   (A0)+,(A1)+
	DBRA.S   D0,memcpy_loop
	BRA.S    loop_bottom
	
c0_zero_c1_positive:
	LSL.W    #8,D0     // * 256
	OR.B     (A0)+,D0  // + c2
	MOVE.B   (A0)+,D1  // c3
	
	BRA.S    call_memset
	
c0_nonzero:
	MOVE.B   D0,D1
	MOVEQ.L  #0,D0
	MOVE.B   D2,D0
	
call_memset:
	SUBQ.W   #1,D0
// memset
	MOVEQ.L  #0,D2
	MOVE.B   D1,D2
	BEQ.S    fill_ready
	LSL.W    #8,D2
	MOVE.B   D1,D2
	
	MOVE.W   D2,D1
	SWAP     D2
	MOVE.W   D1,D2
	
fill_ready:
	LSR.W    #1,D0
	BCC.S    memset_even_4x
	MOVE.W   D2,(A1)+
memset_even_4x:
	
memset_loop:
	MOVE.L   D2,(A1)+
	DBRA.S   D0,memset_loop
	
loop_bottom:
	CMPA.L   4(SP),A1
	BNE.S    loop_top
	
	RTS
}

#else

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

#endif  // #ifdef __MC68K__

}  // namespace damogran
