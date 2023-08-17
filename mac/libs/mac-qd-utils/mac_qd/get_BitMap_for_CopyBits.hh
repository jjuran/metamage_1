/*
	get_BitMap_for_CopyBits.hh
	--------------------------
*/

#ifndef MACQD_GETBITMAPFORCOPYBITS_HH
#define MACQD_GETBITMAPFORCOPYBITS_HH

// Mac OS X
#ifdef __APPLE__
#include <Carbon/Carbon.h>
#endif

// Mac OS
#ifndef __MACWINDOWS__
#include <MacWindows.h>
#endif
#ifndef __QUICKDRAW__
#include <Quickdraw.h>
#endif

// missing-macos
#ifdef MAC_OS_X_VERSION_10_7
#ifndef MISSING_QUICKDRAW_H
#include "missing/Quickdraw.h"
#endif
#endif


namespace mac {
namespace qd  {

inline
const BitMap* get_BitMap_for_CopyBits( const BitMap& bitmap )
{
	return &bitmap;
}

inline
const BitMap* get_BitMap_for_CopyBits( const PixMap& pixmap )
{
	return (const BitMap*) &pixmap;
}

inline
const BitMap* get_BitMap_for_CopyBits( CGrafPtr port )
{
	return GetPortBitMapForCopyBits( port );
}

inline
const BitMap* get_BitMap_for_CopyBits( WindowRef window )
{
	CGrafPtr port = GetWindowPort( window );
	
	return GetPortBitMapForCopyBits( port );
}

}
}

#endif
