/*
	HIQuickDraw.hh
	--------------
	
	Copyright 2017 Josh Juran
*/

#ifndef GENIE_UTILITIES_HIQUICKDRAW_HH
#define GENIE_UTILITIES_HIQUICKDRAW_HH

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
#ifndef MAC_OS_X_VERSION_10_7
#ifndef __QDOFFSCREEN__
#include <QDOffscreen.h>
#endif
#endif
#ifndef __QUICKDRAW__
#include <Quickdraw.h>
#endif


namespace Genie
{
	
	void HIViewDrawBitMap( CGContextRef   context,
	                       CGRect         bounds,
	                       const BitMap&  bitmap );
	
	void HIViewDrawPixMap( CGContextRef  context,
	                       CGRect        bounds,
	                       PixMapHandle  pix );
	
	void HIViewDrawGWorld( CGContextRef  context,
	                       CGRect        bounds,
	                       GWorldPtr     gworld );
	
}

#endif
