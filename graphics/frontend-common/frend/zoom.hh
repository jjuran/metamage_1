/*
	zoom.hh
	-------
*/

#ifndef FREND_ZOOM_HH
#define FREND_ZOOM_HH


namespace frend
{

extern int minimum_zoom_index;
extern int maximum_zoom_index;
extern int current_zoom_index;

void cap_zoom_index( int window_X, int window_Y, int screen_X, int screen_Y );

}

#endif
