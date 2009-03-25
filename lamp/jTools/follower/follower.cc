/*	===========
 *	follower.cc
 *	===========
 */

// Standard C/C++
#include <cstdio>

// POSeven
#include "POSeven/extras/slurp.hh"
#include "POSeven/functions/ftruncate.hh"
#include "POSeven/functions/lseek.hh"
#include "POSeven/functions/write.hh"

// Orion
#include "Orion/GetOptions.hh"
#include "Orion/Main.hh"


namespace tool
{
	
	namespace p7 = poseven;
	namespace O = Orion;
	
	
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
		
		O::BindOption( "--sleep", sleep_arg );
		
		O::GetOptions( argc, argv );
		
		char const *const *free_args = O::FreeArguments();
		
		std::size_t n_args = O::FreeArgumentCount();
		
		const char* pathname = EvaluateMetaFilename( free_args[0] ? free_args[0] : "-" );
		
		float sleep_time = 1.0;
		
		if ( sleep_arg )
		{
			int scanned = std::sscanf( sleep_arg, "%f", &sleep_time );
		}
		
		unsigned long seconds     = sleep_time;
		unsigned long nanoseconds = (sleep_time - seconds) * 1000 * 1000 * 1000;
		
		timespec time = { seconds, nanoseconds };
		
		std::string output;
		std::string previous;
		
	again:
		
		output = p7::slurp( pathname );
		
		if ( output != previous )
		{
			p7::write( p7::stdout_fileno, output );
			
			std::swap( output, previous );
			
			p7::ftruncate( p7::stdout_fileno, p7::lseek( p7::stdout_fileno ) );
			
			p7::lseek( p7::stdout_fileno, 0 );
		}
		
		nanosleep( &time, NULL );
		
		goto again;
		
		return 0;
	}
	
}

