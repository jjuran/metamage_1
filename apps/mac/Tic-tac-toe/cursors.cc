/*
	cursors.cc
	----------
*/

#include "cursors.hh"

// Mac OS X
#ifdef __APPLE__
#include <ApplicationServices/ApplicationServices.h>
#endif

// Mac OS
#ifndef __QUICKDRAW__
#include <Quickdraw.h>
#endif

// missing-macos
#ifdef MAC_OS_X_VERSION_10_7
#ifndef MISSING_QUICKDRAW_H
#include "missing/Quickdraw.h"
#endif
#endif


#ifdef __LITTLE_ENDIAN__

#define PACK16( _15, _14, _13,_12,  \
                _11, _10,  _9, _8,  \
                 _7,  _6,  _5, _4,  \
                 _3,  _2,  _1, _0 ) \
	(short)(                        \
		+ (_15 <<  8)  \
		| (_14 <<  9)  \
		| (_13 << 10)  \
		| (_12 << 11)  \
		| (_11 << 12)  \
		| (_10 << 13)  \
		| ( _9 << 14)  \
		| ( _8 << 15)  \
		| ( _7 <<  0)  \
		| ( _6 <<  1)  \
		| ( _5 <<  2)  \
		| ( _4 <<  3)  \
		| ( _3 <<  4)  \
		| ( _2 <<  5)  \
		| ( _1 <<  6)  \
		| ( _0 <<  7)  \
	)

#else

#define PACK16( _15, _14, _13,_12,  \
                _11, _10,  _9, _8,  \
                 _7,  _6,  _5, _4,  \
                 _3,  _2,  _1, _0 ) \
	(short)(                        \
		+ (_15 << 15)  \
		| (_14 << 14)  \
		| (_13 << 13)  \
		| (_12 << 12)  \
		| (_11 << 11)  \
		| (_10 << 10)  \
		| ( _9 <<  9)  \
		| ( _8 <<  8)  \
		| ( _7 <<  7)  \
		| ( _6 <<  6)  \
		| ( _5 <<  5)  \
		| ( _4 <<  4)  \
		| ( _3 <<  3)  \
		| ( _2 <<  2)  \
		| ( _1 <<  1)  \
		| ( _0 <<  0)  \
	)

#endif  // #ifdef __LITTLE_ENDIAN__

#define _ 0
#define X 1

#define X_DATA  \
{  \
	PACK16( _,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_ ),  \
	PACK16( _,_,X,_,_,_,_,_,_,_,_,_,_,X,_,_ ),  \
	PACK16( _,X,X,X,_,_,_,_,_,_,_,_,X,X,X,_ ),  \
	PACK16( _,_,X,X,X,_,_,_,_,_,_,X,X,X,_,_ ),  \
	PACK16( _,_,_,X,X,X,_,_,_,_,X,X,X,_,_,_ ),  \
	PACK16( _,_,_,_,X,X,X,_,_,X,X,X,_,_,_,_ ),  \
	PACK16( _,_,_,_,_,X,X,X,X,X,X,_,_,_,_,_ ),  \
	PACK16( _,_,_,_,_,_,X,X,X,X,_,_,_,_,_,_ ),  \
	PACK16( _,_,_,_,_,_,X,X,X,X,_,_,_,_,_,_ ),  \
	PACK16( _,_,_,_,_,X,X,X,X,X,X,_,_,_,_,_ ),  \
	PACK16( _,_,_,_,X,X,X,_,_,X,X,X,_,_,_,_ ),  \
	PACK16( _,_,_,X,X,X,_,_,_,_,X,X,X,_,_,_ ),  \
	PACK16( _,_,X,X,X,_,_,_,_,_,_,X,X,X,_,_ ),  \
	PACK16( _,X,X,X,_,_,_,_,_,_,_,_,X,X,X,_ ),  \
	PACK16( _,_,X,_,_,_,_,_,_,_,_,_,_,X,_,_ ),  \
	PACK16( _,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_ ),  \
}

#define X_MASK  \
{  \
	PACK16( _,_,X,_,_,_,_,_,_,_,_,_,_,X,_,_ ),  \
	PACK16( _,X,X,X,_,_,_,_,_,_,_,_,X,X,X,_ ),  \
	PACK16( X,X,X,X,X,_,_,_,_,_,_,X,X,X,X,X ),  \
	PACK16( _,X,X,X,X,X,_,_,_,_,X,X,X,X,X,_ ),  \
	PACK16( _,_,X,X,X,X,X,_,_,X,X,X,X,X,_,_ ),  \
	PACK16( _,_,_,X,X,X,X,X,X,X,X,X,X,_,_,_ ),  \
	PACK16( _,_,_,_,X,X,X,X,X,X,X,X,_,_,_,_ ),  \
	PACK16( _,_,_,_,_,X,X,X,X,X,X,_,_,_,_,_ ),  \
	PACK16( _,_,_,_,_,X,X,X,X,X,X,_,_,_,_,_ ),  \
	PACK16( _,_,_,_,X,X,X,X,X,X,X,X,_,_,_,_ ),  \
	PACK16( _,_,_,X,X,X,X,X,X,X,X,X,X,_,_,_ ),  \
	PACK16( _,_,X,X,X,X,X,_,_,X,X,X,X,X,_,_ ),  \
	PACK16( _,X,X,X,X,X,_,_,_,_,X,X,X,X,X,_ ),  \
	PACK16( X,X,X,X,X,_,_,_,_,_,_,X,X,X,X,X ),  \
	PACK16( _,X,X,X,_,_,_,_,_,_,_,_,X,X,X,_ ),  \
	PACK16( _,_,X,_,_,_,_,_,_,_,_,_,_,X,_,_ ),  \
}

#define O_DATA  \
{  \
	PACK16( _,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_ ),  \
	PACK16( _,_,_,_,_,_,X,X,X,X,_,_,_,_,_,_ ),  \
	PACK16( _,_,_,_,X,X,X,X,X,X,X,X,_,_,_,_ ),  \
	PACK16( _,_,_,X,X,X,X,_,_,X,X,X,X,_,_,_ ),  \
	PACK16( _,_,X,X,X,_,_,_,_,_,_,X,X,X,_,_ ),  \
	PACK16( _,_,X,X,_,_,_,_,_,_,_,_,X,X,_,_ ),  \
	PACK16( _,X,X,X,_,_,_,_,_,_,_,_,X,X,X,_ ),  \
	PACK16( _,X,X,_,_,_,_,_,_,_,_,_,_,X,X,_ ),  \
	PACK16( _,X,X,_,_,_,_,_,_,_,_,_,_,X,X,_ ),  \
	PACK16( _,X,X,X,_,_,_,_,_,_,_,_,X,X,X,_ ),  \
	PACK16( _,_,X,X,_,_,_,_,_,_,_,_,X,X,_,_ ),  \
	PACK16( _,_,X,X,X,_,_,_,_,_,_,X,X,X,_,_ ),  \
	PACK16( _,_,_,X,X,X,X,_,_,X,X,X,X,_,_,_ ),  \
	PACK16( _,_,_,_,X,X,X,X,X,X,X,X,_,_,_,_ ),  \
	PACK16( _,_,_,_,_,_,X,X,X,X,_,_,_,_,_,_ ),  \
	PACK16( _,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_ ),  \
}

#define O_MASK  \
{  \
	PACK16( _,_,_,_,_,_,X,X,X,X,_,_,_,_,_,_ ),  \
	PACK16( _,_,_,_,X,X,X,X,X,X,X,X,_,_,_,_ ),  \
	PACK16( _,_,_,X,X,X,X,X,X,X,X,X,X,_,_,_ ),  \
	PACK16( _,_,X,X,X,X,X,X,X,X,X,X,X,X,_,_ ),  \
	PACK16( _,X,X,X,X,X,X,_,_,X,X,X,X,X,X,_ ),  \
	PACK16( _,X,X,X,X,_,_,_,_,_,_,X,X,X,X,_ ),  \
	PACK16( X,X,X,X,X,_,_,_,_,_,_,X,X,X,X,X ),  \
	PACK16( X,X,X,X,_,_,_,_,_,_,_,_,X,X,X,X ),  \
	PACK16( X,X,X,X,_,_,_,_,_,_,_,_,X,X,X,X ),  \
	PACK16( X,X,X,X,X,_,_,_,_,_,_,X,X,X,X,X ),  \
	PACK16( _,X,X,X,X,_,_,_,_,_,_,X,X,X,X,_ ),  \
	PACK16( _,X,X,X,X,X,X,_,_,X,X,X,X,X,X,_ ),  \
	PACK16( _,_,X,X,X,X,X,X,X,X,X,X,X,X,_,_ ),  \
	PACK16( _,_,_,X,X,X,X,X,X,X,X,X,X,_,_,_ ),  \
	PACK16( _,_,_,_,X,X,X,X,X,X,X,X,_,_,_,_ ),  \
	PACK16( _,_,_,_,_,_,X,X,X,X,_,_,_,_,_,_ ),  \
}

const Cursor X_cursor =
{
	X_DATA,
	X_MASK,
	{ 8, 8 }
};

const Cursor O_cursor =
{
	O_DATA,
	O_MASK,
	{ 8, 8 }
};
