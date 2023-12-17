/*
	Picture-bits.hh
	---------------
*/

#ifndef PICTUREBITS_HH
#define PICTUREBITS_HH

typedef unsigned char Byte;

typedef char** Handle;

struct BitMap;
struct MacRegion;
struct Rect;

const Byte* draw_bits( const Byte* p, bool has_region );

void save_bits_to_picture( Handle         picSave,
                           const BitMap*  srcBits,
                           const Rect*    srcRect,
                           const Rect*    dstRect,
                           short          mode,
                           MacRegion**    maskRgn );

#endif
