/*
	CopyBits.hh
	-----------
*/

#ifndef COPYBITS_HH
#define COPYBITS_HH

struct BitMap;
struct MacRegion;
struct Rect;

pascal void StdBits_patch( const BitMap*  srcBits,
                           const Rect*    srcRect,
                           const Rect*    dstRect,
                           short          mode,
                           MacRegion**    maskRgn );

pascal void CopyBits_patch( const BitMap*  srcBits,
                            const BitMap*  dstBits,
                            const Rect*    srcRect,
                            const Rect*    dstRect,
                            short          mode,
                            MacRegion**    maskRgn );

pascal void ScrollRect_patch( const Rect*  rect,
                              short        dh,
                              short        dv,
                              MacRegion**  updateRgn );

#endif
