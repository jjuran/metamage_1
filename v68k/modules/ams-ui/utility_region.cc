/*
	utility_region.cc
	-----------------
*/

#include "utility_region.hh"

// Mac OS
#ifndef __QUICKDRAW__
#include <Quickdraw.h>
#endif


RgnHandle rectangular_utility_region()
{
	static RgnHandle rgn = NewRgn();
	
	return rgn;
}
