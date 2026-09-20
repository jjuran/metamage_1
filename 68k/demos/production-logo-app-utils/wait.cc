/*
	wait.cc
	-------
*/

#include "wait.hh"

// Mac OS
#ifndef __EVENTS__
#include <Events.h>
#endif

// production-logo-app-utils
#include "terminate.hh"


void wait_or_exit( unsigned ticks )
{
	const EventMask mask = mDownMask | keyDownMask;
	
	EventRecord event;
	
	if ( WaitNextEvent( mask, &event, ticks, NULL ) )
	{
		bool quitting = false;
		
		if ( event.what == keyDown )
		{
			if ( (char) event.message == 'q'  &&  event.modifiers & cmdKey )
			{
				quitting = true;
			}
		}
		
		terminate( quitting );
	}
}
