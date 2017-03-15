/*
	Rect-utils.cc
	-------------
*/

#include "Rect-utils.hh"

// Mac OS
#ifndef __QUICKDRAW__
#include <Quickdraw.h>
#endif


bool empty_rect( const Rect& r )
{
	if ( r.top >= r.bottom )  return true;
	if ( r.left >= r.right )  return true;
	
	return false;
}
