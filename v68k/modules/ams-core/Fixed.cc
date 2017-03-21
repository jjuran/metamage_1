/*
	Fixed.cc
	--------
*/

#include "Fixed.hh"

// quickdraw
#include "qd/fixed.hh"


using namespace quickdraw;


pascal long FixMul_patch( long a, long b )
{
	return fix_mul( a, b );
}

pascal long FixRatio_patch( short numer, short denom )
{
	return fix_ratio( numer, denom );
}

pascal short FixRound_patch( long x )
{
	return fix_round( x );
}
