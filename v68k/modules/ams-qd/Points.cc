/*
	Points.cc
	---------
*/

#include "Points.hh"

// Mac OS
#ifndef __QUICKDRAW__
#include <Quickdraw.h>
#endif


pascal void SetPt_patch( Point* pt, short h, short v )
{
	pt->h = h;
	pt->v = v;
}
