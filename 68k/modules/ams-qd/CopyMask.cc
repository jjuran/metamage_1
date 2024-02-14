/*
	CopyMask.cc
	-----------
*/

#include "CopyMask.hh"

// Mac OS
#ifndef __QUICKDRAW__
#include <Quickdraw.h>
#endif

// ams-common
#include "scoped_zone.hh"


#pragma exceptions off


pascal void CopyMask_patch( const BitMap*  srcBits,
                            const BitMap*  maskBits,
                            const BitMap*  dstBits,
                            const Rect*    srcRect,
                            const Rect*    maskRect,
                            const Rect*    dstRect )
{
	static RgnHandle maskRgn = (scoped_zone(), NewRgn());
	
	BitMapToRegion( maskRgn, maskBits );
	
	OffsetRgn( maskRgn,
	           dstRect->left - srcRect->left,
	           dstRect->top  - srcRect->top );
	
	CopyBits( srcBits,  dstBits, srcRect,  dstRect, srcCopy, maskRgn );
}
