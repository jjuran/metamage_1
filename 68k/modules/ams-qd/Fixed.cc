/*
	Fixed.cc
	--------
*/

#include "Fixed.hh"

// ams-common
#include "callouts.hh"


pascal long FixMul_patch( long a, long b )
{
	return fast_fix_mul( a, b );
}

pascal long FixRatio_patch( short numer, short denom )
{
	return fast_fix_ratio( numer, denom );
}

pascal short FixRound_patch( long x )
{
	return fast_fix_round( x );
}

pascal short HiWord_patch( long x )
{
	return *(short*) &x;
}

pascal short LoWord_patch( long x )
{
	return x;
}

pascal void LongMul_patch( long a, long b, struct Int64Bit* product )
{
	*(long long*) product = (long long) a * b;
}

pascal long FixDiv_patch( long a, long b )
{
	return fast_fix_div( a, b );
}
