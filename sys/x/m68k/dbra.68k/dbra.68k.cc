/*
	dbra.68k.cc
	-----------
*/

#ifndef __MC68K__
#error This unit is for 68K only.
#endif

// Standard C
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>


static
asm void dbra_loop( uint16_t n : __D0 )
{
loop:
	
	DBRA     D0,loop
	
	RTS
}

int main( int argc, char** argv )
{
	int n = argc > 1 ? atoi( argv[ 1 ] ) : 0xFFFF;
	
	/*
		clock() is supposed to measure CPU time, but in MacRelix's libc,
		it's wall clock time, which is what we want here.
	*/
	
	clock_t t0 = clock();
	
	dbra_loop( n );
	
	clock_t t1 = clock();
	
	uint32_t d = t1 - t0;
	
	printf( "%lu\n", d );
	
	return 0;
}
