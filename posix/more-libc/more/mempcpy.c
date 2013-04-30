/*
	mempcpy.c
	---------
*/

// more-libc
#include "more/string.h"


#ifdef __MC68K__

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

#else

void* mempcpy( void* dest, const void* src, size_t n )
{
	char const* p = (char const*) src;
	char      * q = (char      *) dest;
	
	const char* const end = q + n;
	
	while ( q != end )
	{
		*q++ = *p++;
	}
	
	return q;
}

#endif

