/*
	utility_region.cc
	-----------------
*/

#include "utility_region.hh"

// Mac OS
#ifndef __QUICKDRAW__
#include <Quickdraw.h>
#endif

// ams-common
#include "scoped_zone.hh"


#pragma exceptions off


RgnHandle rectangular_utility_region( const Rect& rect )
{
	static RgnHandle rgn = (scoped_zone(), NewRgn());
	
	rgn[0]->rgnBBox = rect;
	
	return rgn;
}
