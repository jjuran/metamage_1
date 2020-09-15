/*
	time.cc
	-------
*/

// Mac OS
#ifndef __DATETIMEUTILS__
#include <DateTimeUtils.h>
#endif

// Standard C
#include <time.h>

// mac-types
#include "mac_types/epoch.hh"


time_t time( time_t* result )
{
	UInt32 dateTime;
	
	GetDateTime( &dateTime );
	
	const time_t now = dateTime - mac::types::epoch_delta();
	
	if ( result )
	{
		*result = now;
	}
	
	return now;
}
