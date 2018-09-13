/*
	Points.cc
	---------
*/

#include "Points.hh"

// Mac OS
#ifndef __QUICKDRAW__
#include <Quickdraw.h>
#endif


pascal void SubPt_patch( Point src, Point* dst )
{
	dst->v -= src.v;
	dst->h -= src.h;
}

pascal void SetPt_patch( Point* pt, short h, short v )
{
	pt->h = h;
	pt->v = v;
}
