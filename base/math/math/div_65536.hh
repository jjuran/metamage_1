/*
	div_65536.hh
	------------
*/

#ifndef MATH_DIV65536_HH
#define MATH_DIV65536_HH


namespace math      {
namespace fractions {

#ifdef __MC68K__

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
unsigned long long* div_65536_ULL( unsigned long long* x )
{
	*x /= 65536;
	
	return x;
}

#endif

}
}

#endif
