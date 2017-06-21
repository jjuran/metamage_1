/*
	graphics.hh
	-----------
*/

#ifndef GRAPHICS_HH
#define GRAPHICS_HH

class bitmap;

extern bool alt_head;
extern bool alt_body;
extern bool alt_wake;

extern int alt_head_x;
extern int alt_head_y;

void draw_frame( bitmap& bits, int dt );

#endif
