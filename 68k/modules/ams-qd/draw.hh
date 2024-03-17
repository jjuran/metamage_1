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

void blit_masked_column( Ptr    src,
                         short  src_stride,
                         Ptr    dst,
                         short  dst_stride,
                         short  height,
                         Byte   mask,
                         short  transfer_mode_AND_0x07 );

void draw_masked_column( Ptr    src,
                         short  src_height,
                         short  src_index,
                         Ptr    dst,
                         short  dst_height,
                         short  dst_stride,
                         Byte   mask,
                         short  transfer_mode_AND_0x07 );

#endif
