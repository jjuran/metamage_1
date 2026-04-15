/*
	zoom.cc
	-------
*/

#include "frend/zoom.hh"


namespace frend
{

int minimum_zoom_index = 0x080 >> 7;  //  50%
int maximum_zoom_index;
int current_zoom_index = 0x100 >> 7;  // 100%

void cap_zoom_index( int window_X, int window_Y, int screen_X, int screen_Y )
{
	long X_zoom = (screen_X << 16) / window_X;
	long Y_zoom = (screen_Y << 16) / window_Y;
	
	// The maximum zoom is the *lesser* of the max X zoom and max Y zoom.
	
	long max = X_zoom < Y_zoom ? X_zoom : Y_zoom;
	
	maximum_zoom_index = max >> 15;
}

}
