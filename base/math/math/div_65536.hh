/*
	div_65536.hh
	------------
*/

#ifndef MATH_DIV65536_HH
#define MATH_DIV65536_HH


namespace math      {
namespace fractions {

inline
short div_65536( short x )
{
	return 0;
}

#ifdef __MC68K__

#pragma parameter div_65536( __D0 )

inline
asm
signed long div_65536( signed long x )
{
	BPL.S    no_add
	
	ADDI.L   #0xFFFF,D0
	
no_add:
	SWAP     D0
	EXT.L    D0
}

#pragma parameter div_65536( __D0 )

inline
signed int div_65536( signed int x )
{
	return __option( fourbyteints ) ? div_65536( (long)  x )
	                                : div_65536( (short) x );
}

#pragma parameter div_65536_ULL( __A0 )

inline
asm
unsigned long long* div_65536_ULL( unsigned long long* x )
{
	MOVE.L   (A0)+,D1
	MOVE.L   (A0),D0
	
	MOVE.W   D1,D0
	SWAP     D0
	
	CLR.W    D1
	SWAP     D1
	
	MOVE.L   D0,(A0)
	MOVE.L   D1,-(A0)
}

#else

inline
signed int div_65536( signed int x )
{
	return x / 65536;
}

inline
signed long div_65536( signed long x )
{
	return x / 65536;
}

inline
unsigned long long* div_65536_ULL( unsigned long long* x )
{
	*x /= 65536;
	
	return x;
}

#endif

inline
signed long long div_65536( signed long long x )
{
	return x / 65536;
}

inline
unsigned int div_65536( unsigned int x )
{
	return x / 65536;
}

inline
unsigned long div_65536( unsigned long x )
{
	return x / 65536;
}

inline
unsigned long long div_65536( unsigned long long x )
{
	return x / 65536;
}

}
}

#endif
