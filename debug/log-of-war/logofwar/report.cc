/*
	report.cc
	---------
*/

#include "logofwar/report.hh"

// Standard C
#include <string.h>

// log-of-war
#include "logofwar/clock.hh"
#include "logofwar/print.hh"


namespace logofwar
{

bool verbose_reports;

void report( diagnostic_level level, const char* file, int line )
{
	if ( verbose_reports )
	{
		print( "\n" );
		print( file );
		print( ":" );
		print( line );
		print( "\n" );
	}
	
	unsigned long long t = runclock();
	
	unsigned long ts = t / 1000;
	unsigned     tms = t % 1000;
	
	const char* level_name = diagnostic_level_names[ level ];
	
	const char* space = &"     "[ strlen( level_name ) - 4 ];
	
	print( "T=" );
	print( ts );
	print( "." );
	print( tms / 100     );
	print( tms / 10 % 10 );
	print( tms      % 10 );
	print( "s  " );
	print( level_name );
	print( ":" );
	print( space );
}

}
