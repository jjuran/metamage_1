/*
	commandmode_state.cc
	--------------------
*/

#include "frend/commandmode_state.hh"

// frontend-common
#include "frend/zoom.hh"


namespace frend
{

CommandMode_state commandmode_state;

bool fullscreen;
bool sharp_pixels;

bool Q_hit;
bool X_hit;

bool commandmode_key( char c )
{
	switch ( c )
	{
		case '0':
			fullscreen = ! fullscreen;
			break;
		
		case '\\':
			sharp_pixels = ! sharp_pixels;
			break;
		
		case 'q':  Q_hit = true;  break;
		case 'x':  X_hit = true;  break;
		
		case '-':
			if ( active_scale->current > active_scale->minimum )
			{
				--active_scale->current;
			}
			break;
		
		case '=':  // +
			if ( active_scale->current < active_scale->maximum )
			{
				++active_scale->current;
			}
			break;
		
		default:
			return false;
	}
	
	return true;
}

}
