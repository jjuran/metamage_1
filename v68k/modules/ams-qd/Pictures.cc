/*
	Pictures.cc
	-----------
*/

#include "Pictures.hh"

// Mac OS
#ifndef __QUICKDRAW__
#include <Quickdraw.h>
#endif


pascal void DrawPicture_patch( PicHandle pic, const Rect* dstRect )
{
}
