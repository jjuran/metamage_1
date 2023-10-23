/*
	StdBits.hh
	----------
*/

#ifndef STDBITS_HH
#define STDBITS_HH

struct BitMap;
struct MacRegion;
struct Rect;

pascal void StdBits_patch( const BitMap*  srcBits,
                           const Rect*    srcRect,
                           const Rect*    dstRect,
                           short          mode,
                           MacRegion**    maskRgn );

#endif
