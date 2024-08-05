/*
	time.cc
	-------
*/

// Standard C
#include <time.h>

// mac-types
#include "mac_types/epoch.hh"

// mac-glue-utils
#include "mac_glue/DateTimeUtils.hh"


time_t time( time_t* result )
{
	const time_t now = mac::glue::get_Time() - mac::types::epoch_delta();
	
	if ( result )
	{
		*result = now;
	}
	
	return now;
}
