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


struct PixMap;

CGImageRef CreateCGImageFromPixMap( const PixMap& pixmap );

#endif
