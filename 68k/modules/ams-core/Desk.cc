/*
	Desk.cc
	-------
*/

#include "Desk.hh"

// mac-glue-utils
#include "mac_glue/OSUtils.hh"

// ams-core
#include "throttle.hh"


pascal Boolean SystemEdit_patch( short cmd )
{
	return false;  // There are no DAs yet, so no DA windows
}

pascal void SystemTask_patch()
{
	/*
		If we're in a tight loop calling SystemTask()
		and either GetOSEvent() or OSEventAvail(),
		then wait up to one tick (or less, if an
		event arrives) to throttle use of the CPU.
	*/
	
	if ( ticks_to_wait_in_SystemTask() )
	{
		mac::glue::delay( ~1 );
	}
	
	disable_SystemTask_throttle();
}

pascal Boolean SystemEvent_patch( const EventRecord* event )
{
	return false;
}
