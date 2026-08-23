/*
	zoom.cc
	-------
*/

#include "frend/zoom.hh"


namespace frend
{

scale_indices screen_scale;
scale_indices window_scale;

scale_indices* active_scale;

void cap_zoom_index( int window_X, int window_Y, int screen_X, int screen_Y )
{
	long X_zoom = (screen_X << 16) / window_X;
	long Y_zoom = (screen_Y << 16) / window_Y;
	
	// The maximum zoom is the *lesser* of the max X zoom and max Y zoom.
	
	long max = X_zoom < Y_zoom ? X_zoom : Y_zoom;
	
	active_scale->maximum = max >> 15;
}

}
