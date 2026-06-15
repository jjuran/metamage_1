/*
	d1904z.cc
	---------
*/

#include "d1904z.hh"

// mac-types
#include "mac_types/epoch.hh"


using mac::types::epoch_delta;


typedef unsigned long d1904;
typedef unsigned long d1904z;


time_t time_from_d1904( d1904 local_1904 )
{
	struct tm date;
	
	time_t local_1970 = local_1904 - epoch_delta();
	
	gmtime_r( &local_1970, &date );
	
	time_t global_1970 = mktime( &date );
	
	if ( date.tm_isdst )
	{
		global_1970 -= 3600;  // adjust for DST
	}
	
	return global_1970;
}

d1904z d1904z_from_d1904( d1904 local_1904 )
{
	return time_from_d1904( local_1904 ) + epoch_delta();
}

d1904 d1904_from_time( time_t global_1970 )
{
	struct tm date;
	
	localtime_r( &global_1970, &date );
	
	time_t local_1970 = timegm( &date );
	
	if ( date.tm_isdst )
	{
		local_1970 += 3600;  // adjust for DST
	}
	
	return local_1970 + epoch_delta();
}

d1904 d1904_from_d1904z( d1904z global_1904 )
{
	return d1904_from_time( global_1904 - epoch_delta() );
}
