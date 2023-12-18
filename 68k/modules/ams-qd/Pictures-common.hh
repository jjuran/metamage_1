/*
	Pictures-common.hh
	------------------
*/

#ifndef PICTURESCOMMON_HH
#define PICTURESCOMMON_HH

typedef unsigned char Byte;

struct Rect;

short read_word( const Byte*& p );
Rect  read_Rect( const Byte*& p );

#endif
