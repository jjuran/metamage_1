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
#include "redraw_lock.hh"


#pragma exceptions off


pascal void CopyMask_patch( const BitMap*  srcBits,
                            const BitMap*  maskBits,
                            const BitMap*  dstBits,
                            const Rect*    srcRect,
                            const Rect*    maskRect,
                            const Rect*    dstRect )
{
	redraw_lock lock( *dstBits, *dstRect );
	
	/*
		This approach doesn't clip the source to the mask, contrary to Inside
		Macintosh, Volume IV.  But as long as the source doesn't contain any
		bits outside the mask that are set (which it shouldn't, since Mac OS
		wouldn't be drawing them anyway), we'll produce the correct result.
	*/
	
	CopyBits( maskBits, dstBits, maskRect, dstRect, srcBic, NULL );
	CopyBits( srcBits,  dstBits, srcRect,  dstRect, srcXor, NULL );
}
