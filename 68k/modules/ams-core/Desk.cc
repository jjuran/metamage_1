/*
	Desk.cc
	-------
*/

#include "Desk.hh"

// Mac OS
#ifndef __OSUTILS__
#include <OSUtils.h>
#endif

// ams-core
#include "splode.hh"


QHdr EventQueue : 0x014A;


pascal Boolean SystemEdit_patch( short cmd )
{
	return false;  // There are no DAs yet, so no DA windows
}

pascal void SystemTask_patch()
{
	wait_for_user_input( EventQueue.qFlags & 1 );
}

pascal Boolean SystemEvent_patch( const EventRecord* event )
{
	return false;
}
