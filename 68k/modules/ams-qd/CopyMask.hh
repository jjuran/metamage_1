/*
	CopyMask.hh
	-----------
*/

#ifndef COPYMASK_HH
#define COPYMASK_HH

struct BitMap;
struct Rect;

pascal void CopyMask_patch( const BitMap*  srcBits,
                            const BitMap*  maskBits,
                            const BitMap*  dstBits,
                            const Rect*    srcRect,
                            const Rect*    maskRect,
                            const Rect*    dstRect );

#endif
