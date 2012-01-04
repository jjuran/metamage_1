/*
	time.cc
	-------
*/

// Mac OS
#ifndef __DATETIMEUTILS__
#include <DateTimeUtils.h>
#endif

// POSIX
#include <time.h>

// TimeOff
#include "TimeOff/TimeOff.hh"


time_t time( time_t* result )
{
	const unsigned long delta = TimeOff::MacToUnixTimeDifference();
	
	UInt32 dateTime;
	
	GetDateTime( &dateTime );
	
	const time_t now = dateTime - delta;
	
	if ( result )
	{
		*result = now;
	}
	
	return now;
}

