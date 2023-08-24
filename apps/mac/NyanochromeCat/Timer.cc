/*
	Timer.cc
	--------
*/

#include "Timer.hh"


#pragma exceptions off


timer animation_timer;

bool timer::pulse()
{
	if ( it_is_paused )
	{
		return false;
	}
	
	clock_t now;
	get_clock( &now );
	
	if ( now >= its_next_pulse )
	{
		its_next_pulse += its_pulse_interval;
		
		if ( its_next_pulse <= now )
		{
			its_next_pulse = now + its_pulse_interval;
		}
		
		return true;
	}
	
	return false;
}
