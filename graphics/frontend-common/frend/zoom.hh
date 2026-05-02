/*
	zoom.hh
	-------
*/

#ifndef FREND_ZOOM_HH
#define FREND_ZOOM_HH


namespace frend
{

enum
{
	Zoom_index_1_0 = 0x100 >> 7,          // 100%
	Zoom_index_0_5 = Zoom_index_1_0 / 2,  //  50%
	Zoom_index_2_0 = Zoom_index_1_0 * 2,  // 200%
};

extern int minimum_zoom_index;
extern int maximum_zoom_index;
extern int current_zoom_index;

void cap_zoom_index( int window_X, int window_Y, int screen_X, int screen_Y );

}

#endif
