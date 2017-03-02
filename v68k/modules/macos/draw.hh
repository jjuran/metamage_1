/*
	draw.hh
	-------
*/

#ifndef DRAW_HH
#define DRAW_HH

struct GrafPort;
struct MacRegion;
struct Rect;

/*
	Take the intersection of the given port's clipRgn and visRgn along
	with the bounds of the port's bitmap and the destination rect.
*/

void get_refined_clip_region( const GrafPort&  port,
                              const Rect&      dstRect,
                              MacRegion**      result );

#endif
