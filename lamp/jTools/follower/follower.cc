/*	===========
 *	follower.cc
 *	===========
 */

// Standard C
#include <stdlib.h>
#include <time.h>

// command
#include "command/get_option.hh"

// gear
#include "gear/parse_float.hh"

// poseven
#include "poseven/extras/slurp.hh"
#include "poseven/functions/ftruncate.hh"
#include "poseven/functions/pwrite.hh"

// Orion
#include "Orion/Main.hh"


using namespace command::constants;

enum
{
	Option_last_byte = 255,
	
	Option_sleep,
};

static command::option options[] =
{
	{ "sleep", Option_sleep },
	
	{ NULL }
};

static float sleep_time = 1.0;

static char* const* get_options( char* const* argv )
{
	++argv;  // skip arg 0
	
	short opt;
	
	while ( (opt = command::get_option( &argv, options )) )
	{
		switch ( opt )
		{
			case Option_sleep:
				sleep_time = gear::parse_float( command::global_result.param );
				break;
			
			default:
				abort();
		}
	}
	
	return argv;
}


namespace tool
{
	
	namespace p7 = poseven;
	
	
	static bool PathnameMeansStdIn( const char* pathname )
	{
		return    pathname[0] == '-'
			   && pathname[1] == '\0';
	}
	
	static const char* EvaluateMetaFilename( const char* pathname )
	{
		if ( PathnameMeansStdIn( pathname ) )
		{
			return "/dev/fd/0";
		}
		
		return pathname;
	}
	
	int Main( int argc, char** argv )
	{
		char *const *args = get_options( argv );
		
		const int argn = argc - (args - argv);
		
		const char* pathname = EvaluateMetaFilename( args[0] ? args[0] : "-" );
		
		unsigned long seconds     = sleep_time;
		unsigned long nanoseconds = (sleep_time - seconds) * 1000 * 1000 * 1000;
		
		timespec time = { seconds, nanoseconds };
		
		plus::string output;
		plus::string previous;
		
	again:
		
		output = p7::slurp( pathname );
		
		if ( output != previous )
		{
			p7::pwrite( p7::stdout_fileno, output, 0 );
			
			p7::ftruncate( p7::stdout_fileno, output.size() );
			
			swap( output, previous );
		}
		
		nanosleep( &time, NULL );
		
		goto again;
		
		return 0;
	}
	
}
