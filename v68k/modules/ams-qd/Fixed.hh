/*
	Fixed.hh
	--------
*/

#ifndef FIXED_HH
#define FIXED_HH

pascal long FixMul_patch( long a, long b );

pascal long FixRatio_patch( short numer, short denom );

pascal short FixRound_patch( long x );

pascal short HiWord_patch( long x );
pascal short LoWord_patch( long x );

#endif
