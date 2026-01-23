/*
	Desk.cc
	-------
*/

#include "Desk.hh"

// ams-core
#include "splode.hh"
#include "throttle.hh"


pascal Boolean SystemEdit_patch( short cmd )
{
	return false;  // There are no DAs yet, so no DA windows
}

pascal void SystemTask_patch()
{
	wait_for_user_input( ticks_to_wait_in_SystemTask() );
}

pascal Boolean SystemEvent_patch( const EventRecord* event )
{
	return false;
}
