/*
	report.cc
	---------
*/

#include "logofwar/report.hh"

// standard C
#include <stdio.h>
#include <string.h>

// log-of-war
#include "logofwar/clock.hh"


namespace logofwar
{

bool verbose_reports;

void report( diagnostic_level level, const char* file, int line )
{
	if ( verbose_reports )
	{
		fprintf( stderr, "\n%s:%d\n", file, line );
	}
	
	unsigned long long t = runclock();
	
	unsigned long ts = t / 1000;
	unsigned     tms = t % 1000;
	
	const char* level_name = diagnostic_level_names[ level ];
	
	const char* space = &"     "[ strlen( level_name ) - 4 ];
	
	fprintf( stderr, "T=%lu.%.3ds  %s:%s", ts, tms, level_name, space );
}

}
