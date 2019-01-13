/*
	Desk.cc
	-------
*/

#include "Desk.hh"

// ams-core
#include "splode.hh"


pascal Boolean SystemEdit_patch( short cmd )
{
	return false;  // There are no DAs yet, so no DA windows
}

pascal void SystemTask_patch()
{
	poll_user_input();
}

pascal Boolean SystemEvent_patch( const EventRecord* event )
{
	return false;
}
