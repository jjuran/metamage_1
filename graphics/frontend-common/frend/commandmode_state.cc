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

bool Q_hit;
bool X_hit;

bool commandmode_key( char c )
{
	switch ( c )
	{
		case 'q':  Q_hit = true;  break;
		case 'x':  X_hit = true;  break;
		
		case '-':
			if ( current_zoom_index > minimum_zoom_index )
			{
				--current_zoom_index;
			}
			break;
		
		case '=':  // +
			if ( current_zoom_index < maximum_zoom_index )
			{
				++current_zoom_index;
			}
			break;
		
		default:
			return false;
	}
	
	return true;
}

}
