/*
	math.hh
	-------
*/

#include "math.hh"


asm
unsigned long fixmulu_w( unsigned short a : __D0,
                         unsigned long  b : __D1 )
{
	MOVE.W   D0,D2    // a
	MULU.W   D1,D2    // a * b.lo
	CLR.W    D2
	SWAP     D2       // (a * b.lo) >> 16
	SWAP     D1       // b.hi
	MULU.W   D1,D0    // a * b.hi
	ADD.L    D2,D0    // a * b.hi + (a * b.lo >> 16)
	
	RTS
}
