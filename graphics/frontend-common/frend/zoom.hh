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

struct scale_indices
{
	int minimum;
	int maximum;
	int current;
};

extern scale_indices screen_scale;
extern scale_indices window_scale;

extern scale_indices* active_scale;

void cap_zoom_index( int window_X, int window_Y, int screen_X, int screen_Y );

}

#endif
