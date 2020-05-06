/*
	utility_region.cc
	-----------------
*/

#include "utility_region.hh"

// Mac OS
#ifndef __QUICKDRAW__
#include <Quickdraw.h>
#endif


RgnHandle rectangular_utility_region( const Rect& rect )
{
	static RgnHandle rgn = NewRgn();
	
	rgn[0]->rgnBBox = rect;
	
	return rgn;
}
