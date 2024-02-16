/*
	draw.hh
	-------
*/

#ifndef DRAW_HH
#define DRAW_HH

struct GrafPort;
struct MacRegion;
struct Rect;

typedef unsigned char Byte;

typedef char* Ptr;

/*
	Take the intersection of the given port's clipRgn and visRgn along
	with the bounds of the port's bitmap and the destination rect.
*/

void get_refined_clip_region( const GrafPort&  port,
                              const Rect&      dstRect,
                              MacRegion**      result );

Ptr draw_masked_byte( Byte   byte,
                      Byte   mask,
                      Ptr    dst,
                      short  transfer_mode_AND_0x07 );

#endif
