/*
	Points.cc
	---------
*/

#include "Points.hh"

// Mac OS
#ifndef __QUICKDRAW__
#include <Quickdraw.h>
#endif


pascal void AddPt_patch( Point src, Point* dst )
{
	dst->v += src.v;
	dst->h += src.h;
}

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

pascal unsigned char EqualPt_patch( Point a, Point b )
{
	return *(long*) &a == *(long*) &b;
}
