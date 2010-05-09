/*	===========
 *	follower.cc
 *	===========
 */

// Standard C/C++
#include <cstdio>

// poseven
#include "poseven/extras/slurp.hh"
#include "poseven/functions/ftruncate.hh"
#include "poseven/functions/pwrite.hh"

// Orion
#include "Orion/get_options.hh"
#include "Orion/Main.hh"


namespace tool
{
	
	namespace p7 = poseven;
	namespace o = orion;
	
	
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
	
	int Main( int argc, iota::argv_t argv )
	{
		const char* sleep_arg = NULL;
		
		o::bind_option_to_variable( "--sleep", sleep_arg );
		
		o::get_options( argc, argv );
		
		char const *const *free_args = o::free_arguments();
		
		std::size_t n_args = o::free_argument_count();
		
		const char* pathname = EvaluateMetaFilename( free_args[0] ? free_args[0] : "-" );
		
		float sleep_time = 1.0;
		
		if ( sleep_arg )
		{
			int scanned = std::sscanf( sleep_arg, "%f", &sleep_time );
		}
		
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
			
			std::swap( output, previous );
		}
		
		nanosleep( &time, NULL );
		
		goto again;
		
		return 0;
	}
	
}

