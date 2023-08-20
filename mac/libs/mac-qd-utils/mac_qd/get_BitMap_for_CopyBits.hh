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
#ifndef __QUICKDRAW__
#include <Quickdraw.h>
#endif

// This is only needed for GetWindowPort().
#if OPAQUE_TOOLBOX_STRUCTS
#ifndef __MACWINDOWS__
#include <MacWindows.h>
#endif
#endif

// missing-macos
#if ! __LP64__
#ifdef MAC_OS_X_VERSION_10_7
#ifndef MISSING_QUICKDRAW_H
#include "missing/Quickdraw.h"
#endif
#endif
#endif


#if ! __LP64__

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
#if OPAQUE_TOOLBOX_STRUCTS
	
	CGrafPtr port = GetWindowPort( window );
	
	return GetPortBitMapForCopyBits( port );
	
#else
	
	return &window->portBits;
	
#endif
}

}
}

#endif  // #if ! __LP64__

#endif
