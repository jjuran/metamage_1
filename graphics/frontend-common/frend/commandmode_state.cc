/*
	commandmode_state.cc
	--------------------
*/

#include "frend/commandmode_state.hh"


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
		
		default:
			return false;
	}
	
	return true;
}

}
