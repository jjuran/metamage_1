/*	========
 *	sleep.cc
 *	========
 */

// POSIX
#include <time.h>
#include <unistd.h>

// Standard C
#include <math.h>

// must
#include "must/write.h"

// Iota
#include "iota/strings.hh"

// gear
#include "gear/parse_float.hh"

// more-posix
#include "more/perror.hh"


int main( int argc, char const *const argv[] )
{
	// Check for correct number of args
	if ( argc != 2 )
	{
		must_write( STDERR_FILENO, STR_LEN( "Usage: sleep seconds\n" ) );
		
		return 2;
	}
	
	const char* sleep_arg = argv[1];
	
	const float sleep_time = gear::parse_float( &sleep_arg );
	
	if ( const bool parsed = sleep_arg != argv[1] )
	{
		const float int_sleep_time = floor( sleep_time );
		
		const long seconds     = static_cast< long >( int_sleep_time );
		const long nanoseconds = static_cast< long >( (sleep_time - int_sleep_time) * 1000 * 1000 * 1000 );
		
		const timespec time = { seconds, nanoseconds };
		
		nanosleep( &time, NULL );
	}
	else
	{
		more::perror( "sleep: invalid time interval", argv[1], 0 );
		
		return 1;
	}
	
	return 0;
}
