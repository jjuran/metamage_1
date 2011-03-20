/*
	mempcpy.68k.c
	-------------
*/

#ifdef __MC68K__

// more-posix
#include "more/string.h"


asm void* mempcpy( void* dest, const void* src, size_t n )
{
	MOVEA.L  4(SP),A0  ; // dest
	MOVEA.L  8(SP),A1  ; // src
	MOVE.L  12(SP),D0  ; // n
	
	BEQ.S   end
	
loop:
	MOVE.B  (A1)+,(A0)+
	
	SUBQ.L  #1,D0
	
	BNE.S   loop
	
end:
	RTS
}

#endif

