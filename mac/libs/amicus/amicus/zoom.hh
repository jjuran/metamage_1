/*
	zoom.hh
	-------
*/

#ifndef AMICUS_ZOOM_HH
#define AMICUS_ZOOM_HH


namespace amicus
{

const int top_zoom_index = 2 * 4;  // 400%

extern int maximum_zoom_index;
extern int current_zoom_index;

void cap_zoom_index( int window_X, int window_Y, int screen_X, int screen_Y );

unsigned command_ID_for_zoom_index( int zoom );

unsigned command_to_zoom_in();
unsigned command_to_zoom_out();

}

#endif
