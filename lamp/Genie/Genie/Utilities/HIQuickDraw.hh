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


struct BitMap;


namespace Genie
{
	
	void HIViewDrawBitMap( CGContextRef   context,
	                       CGRect         bounds,
	                       const BitMap&  bitmap );
	
}

#endif
