/*
	mempcpy.68k.c
	-------------
*/

#ifdef __MC68K__

// more-libc
#include "more/string.h"


asm void* mempcpy( void* dest : __A0, const void* src : __A1, size_t n : __D0 )
{
	TST.L   D0  ; // n
	
	BEQ.S   end
	
loop:
	MOVE.B  (A1)+,(A0)+
	
	SUBQ.L  #1,D0
	
	BNE.S   loop
	
end:
	RTS
}

#endif

