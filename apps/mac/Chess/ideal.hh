/*
	ideal.hh
	--------
*/

#ifndef CHESS_IDEAL_HH
#define CHESS_IDEAL_HH

enum
{
	ideal_unit_length = 37,
	ideal_2x_length   = ideal_unit_length * 2 - 1,  // 73
};

inline
int ideal_icon_size( int unit_length )
{
	return unit_length < ideal_2x_length ? 32 : 64;
}

#endif
