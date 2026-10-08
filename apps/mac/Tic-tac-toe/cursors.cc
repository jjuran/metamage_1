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

// iota
#include "iota/bits.hh"


#define X_DATA  \
{  \
	BIG16( _,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_ ),  \
	BIG16( _,_,X,_,_,_,_,_,_,_,_,_,_,X,_,_ ),  \
	BIG16( _,X,X,X,_,_,_,_,_,_,_,_,X,X,X,_ ),  \
	BIG16( _,_,X,X,X,_,_,_,_,_,_,X,X,X,_,_ ),  \
	BIG16( _,_,_,X,X,X,_,_,_,_,X,X,X,_,_,_ ),  \
	BIG16( _,_,_,_,X,X,X,_,_,X,X,X,_,_,_,_ ),  \
	BIG16( _,_,_,_,_,X,X,X,X,X,X,_,_,_,_,_ ),  \
	BIG16( _,_,_,_,_,_,X,X,X,X,_,_,_,_,_,_ ),  \
	BIG16( _,_,_,_,_,_,X,X,X,X,_,_,_,_,_,_ ),  \
	BIG16( _,_,_,_,_,X,X,X,X,X,X,_,_,_,_,_ ),  \
	BIG16( _,_,_,_,X,X,X,_,_,X,X,X,_,_,_,_ ),  \
	BIG16( _,_,_,X,X,X,_,_,_,_,X,X,X,_,_,_ ),  \
	BIG16( _,_,X,X,X,_,_,_,_,_,_,X,X,X,_,_ ),  \
	BIG16( _,X,X,X,_,_,_,_,_,_,_,_,X,X,X,_ ),  \
	BIG16( _,_,X,_,_,_,_,_,_,_,_,_,_,X,_,_ ),  \
	BIG16( _,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_ ),  \
}

#define X_MASK  \
{  \
	BIG16( _,_,X,_,_,_,_,_,_,_,_,_,_,X,_,_ ),  \
	BIG16( _,X,X,X,_,_,_,_,_,_,_,_,X,X,X,_ ),  \
	BIG16( X,X,X,X,X,_,_,_,_,_,_,X,X,X,X,X ),  \
	BIG16( _,X,X,X,X,X,_,_,_,_,X,X,X,X,X,_ ),  \
	BIG16( _,_,X,X,X,X,X,_,_,X,X,X,X,X,_,_ ),  \
	BIG16( _,_,_,X,X,X,X,X,X,X,X,X,X,_,_,_ ),  \
	BIG16( _,_,_,_,X,X,X,X,X,X,X,X,_,_,_,_ ),  \
	BIG16( _,_,_,_,_,X,X,X,X,X,X,_,_,_,_,_ ),  \
	BIG16( _,_,_,_,_,X,X,X,X,X,X,_,_,_,_,_ ),  \
	BIG16( _,_,_,_,X,X,X,X,X,X,X,X,_,_,_,_ ),  \
	BIG16( _,_,_,X,X,X,X,X,X,X,X,X,X,_,_,_ ),  \
	BIG16( _,_,X,X,X,X,X,_,_,X,X,X,X,X,_,_ ),  \
	BIG16( _,X,X,X,X,X,_,_,_,_,X,X,X,X,X,_ ),  \
	BIG16( X,X,X,X,X,_,_,_,_,_,_,X,X,X,X,X ),  \
	BIG16( _,X,X,X,_,_,_,_,_,_,_,_,X,X,X,_ ),  \
	BIG16( _,_,X,_,_,_,_,_,_,_,_,_,_,X,_,_ ),  \
}

#define O_DATA  \
{  \
	BIG16( _,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_ ),  \
	BIG16( _,_,_,_,_,_,X,X,X,X,_,_,_,_,_,_ ),  \
	BIG16( _,_,_,_,X,X,X,X,X,X,X,X,_,_,_,_ ),  \
	BIG16( _,_,_,X,X,X,X,_,_,X,X,X,X,_,_,_ ),  \
	BIG16( _,_,X,X,X,_,_,_,_,_,_,X,X,X,_,_ ),  \
	BIG16( _,_,X,X,_,_,_,_,_,_,_,_,X,X,_,_ ),  \
	BIG16( _,X,X,X,_,_,_,_,_,_,_,_,X,X,X,_ ),  \
	BIG16( _,X,X,_,_,_,_,_,_,_,_,_,_,X,X,_ ),  \
	BIG16( _,X,X,_,_,_,_,_,_,_,_,_,_,X,X,_ ),  \
	BIG16( _,X,X,X,_,_,_,_,_,_,_,_,X,X,X,_ ),  \
	BIG16( _,_,X,X,_,_,_,_,_,_,_,_,X,X,_,_ ),  \
	BIG16( _,_,X,X,X,_,_,_,_,_,_,X,X,X,_,_ ),  \
	BIG16( _,_,_,X,X,X,X,_,_,X,X,X,X,_,_,_ ),  \
	BIG16( _,_,_,_,X,X,X,X,X,X,X,X,_,_,_,_ ),  \
	BIG16( _,_,_,_,_,_,X,X,X,X,_,_,_,_,_,_ ),  \
	BIG16( _,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_ ),  \
}

#define O_MASK  \
{  \
	BIG16( _,_,_,_,_,_,X,X,X,X,_,_,_,_,_,_ ),  \
	BIG16( _,_,_,_,X,X,X,X,X,X,X,X,_,_,_,_ ),  \
	BIG16( _,_,_,X,X,X,X,X,X,X,X,X,X,_,_,_ ),  \
	BIG16( _,_,X,X,X,X,X,X,X,X,X,X,X,X,_,_ ),  \
	BIG16( _,X,X,X,X,X,X,_,_,X,X,X,X,X,X,_ ),  \
	BIG16( _,X,X,X,X,_,_,_,_,_,_,X,X,X,X,_ ),  \
	BIG16( X,X,X,X,X,_,_,_,_,_,_,X,X,X,X,X ),  \
	BIG16( X,X,X,X,_,_,_,_,_,_,_,_,X,X,X,X ),  \
	BIG16( X,X,X,X,_,_,_,_,_,_,_,_,X,X,X,X ),  \
	BIG16( X,X,X,X,X,_,_,_,_,_,_,X,X,X,X,X ),  \
	BIG16( _,X,X,X,X,_,_,_,_,_,_,X,X,X,X,_ ),  \
	BIG16( _,X,X,X,X,X,X,_,_,X,X,X,X,X,X,_ ),  \
	BIG16( _,_,X,X,X,X,X,X,X,X,X,X,X,X,_,_ ),  \
	BIG16( _,_,_,X,X,X,X,X,X,X,X,X,X,_,_,_ ),  \
	BIG16( _,_,_,_,X,X,X,X,X,X,X,X,_,_,_,_ ),  \
	BIG16( _,_,_,_,_,_,X,X,X,X,_,_,_,_,_,_ ),  \
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
