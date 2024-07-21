/*
	graphics.hh
	-----------
*/

#ifndef CHESS_GRAPHICS_HH
#define CHESS_GRAPHICS_HH


enum Layers
{
	Layer_mask = 0,
	Layer_unit = 1,
	Layer_all = -1,  // background and unit
};

extern short unit_length;

void draw_square( int square, Layers layers );

void draw_board();

#endif
