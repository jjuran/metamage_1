/*
	apple_events.hh
	---------------
*/

#ifndef AMICUS_APPLEEVENTS_HH
#define AMICUS_APPLEEVENTS_HH


struct AEDesc;

namespace amicus
{

typedef AEDesc AppleEvent;

int handle_Open_event( const AppleEvent* event );

int wait_for_first_Apple_event();

}

#endif
