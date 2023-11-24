/*
	math.hh
	-------
*/

#ifndef MATH_HH
#define MATH_HH

/*
	mulu_w(a, b)       --> a * b
	muldivu_w(a, b, d) --> a * b / d
	fixmulu_w(a, b)    --> a * b >> 16
*/

inline
asm
unsigned long mulu_w( unsigned short a : __D0,
                      unsigned short b : __D1 )
{
	MULU.W   D1,D0
}

inline
asm
unsigned short muldivu_w( unsigned short a : __D0,
                          unsigned short b : __D1,
                          unsigned short d : __D2 )  // divisor
{
	MULU.W   D1,D0
	DIVU.W   D2,D0
}

unsigned long fixmulu_w( unsigned short a : __D0,
                         unsigned long  b : __D1 );

#endif
