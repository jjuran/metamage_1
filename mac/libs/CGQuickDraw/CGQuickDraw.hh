/*
	CGQuickDraw.hh
	--------------
*/

#ifndef CGQUICKDRAW_HH
#define CGQUICKDRAW_HH

// Mac OS X
#ifdef __APPLE__
#include <ApplicationServices/ApplicationServices.h>
#endif

// Mac OS
#ifndef CGCONTEXT_H_
#ifndef __CGCONTEXT__
#include <CGContext.h>
#endif
#endif


struct BitMap;
struct PixMap;

typedef PixMap** PixMapHandle;

CGImageRef CreateCGImageFromBitMap( const BitMap& bitmap );
CGImageRef CreateCGImageFromPixMap( PixMapHandle  pix    );

#endif
