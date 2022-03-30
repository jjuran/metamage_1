/*
	location.cc
	-----------
	
	TODO:
	  * Print fractional values (especially for GMT delta).
	  * Add subcommands for individual fields.
*/

// Mac OS X
#ifdef __APPLE__
#include <CoreServices/CoreServices.h>
#endif

// Mac OS
#ifndef __OSUTILS__
#include <OSUtils.h>
#endif

// Standard C
#include <stdio.h>


static inline
int degrees_from_fract( long fract )
{
	const short fixed_18_14 = fract >> 16;
	
	return fixed_18_14 * 90 / 16384;
}

static inline
int get_degrees_latitude( const MachineLocation& location )
{
	return degrees_from_fract( location.latitude );
}

static inline
int get_degrees_longitude( const MachineLocation& location )
{
	return degrees_from_fract( location.longitude );
}

static
int get_GMT_delta_hours( const MachineLocation& location )
{
	long gmtDelta = location.u.gmtDelta;
	
	if ( gmtDelta & 0x00800000 )
	{
		gmtDelta |= 0xFF000000;
	}
	else
	{
		gmtDelta &= 0x00FFFFFF;
	}
	
	return gmtDelta / 3600;
}

static inline
bool has_daylight_saving_time( const MachineLocation& location )
{
	return location.u.gmtDelta < 0;
}

static
void print_location( const MachineLocation& location )
{
	int latitude  = get_degrees_latitude ( location );
	int longitude = get_degrees_longitude( location );
	int gmtDelta  = get_GMT_delta_hours  ( location );
	
	bool dls = has_daylight_saving_time( location );
	
	#define ENDL "\n"
	
	printf( "Latitude:  %d degrees"    ENDL
	        "Longitude: %d degrees"    ENDL
	        "GMT delta: %d hours"      ENDL
	        "Daylight Saving Time: %s" "\n", latitude,
	                                         longitude,
	                                         gmtDelta,
	                                         dls ? "on" : "off" );
}

int main( int argc, char** argv )
{
	MachineLocation location;
	
	ReadLocation( &location );
	
	print_location( location );
	
	return 0;
}
