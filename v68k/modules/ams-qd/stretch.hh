/*
	stretch.hh
	----------
*/

#ifndef STRETCH_HH
#define STRETCH_HH

struct BitMap;
struct Rect;

void stretch_bits( const BitMap&  srcBits,
                   const BitMap&  dstBits,
                   const Rect&    srcRect,
                   const Rect&    dstRect );

#endif
