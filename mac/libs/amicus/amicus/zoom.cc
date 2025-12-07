/*
	zoom.cc
	-------
*/

#include "amicus/zoom.hh"


namespace amicus
{

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

unsigned command_ID_for_zoom_index( int zoom )
{
	if ( zoom == 1 )
	{
		return ' 50%';
	}
	
	unsigned command_ID = ('0' + (zoom >> 1)) << 24
	                    | "05"[ zoom & 1 ]    << 16
	                    | '0'                 <<  8
	                    | '%';
	
	return command_ID;
}

unsigned command_to_zoom_in()
{
	const int zoom_index = current_zoom_index + 1;
	
	if ( zoom_index > maximum_zoom_index )
	{
		return 0;
	}
	
	return command_ID_for_zoom_index( zoom_index );
}

unsigned command_to_zoom_out()
{
	const int zoom_index = current_zoom_index - 1;
	
	if ( zoom_index == 0 )
	{
		return 0;
	}
	
	return command_ID_for_zoom_index( zoom_index );
}

}
