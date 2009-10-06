/*	======
 *	cat.cc
 *	======
 */

// POSIX
#include <fcntl.h>
#include <unistd.h>

// poseven
#include "poseven/extras/pump.hh"
#include "poseven/functions/open.hh"
#include "poseven/functions/perror.hh"

// Orion
#include "Orion/Main.hh"


namespace tool
{
	
	namespace NN = Nucleus;
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
	
	int Main( int argc, iota::argv_t argv )
	{
		iota::arg_t argv0 = argv[0];
		
		iota::argp_t args = argv + 1;
		
		// Check for sufficient number of args
		if ( *args == NULL )
		{
			static iota::const_argv_t default_args = { "-", NULL };
			
			args = (iota::argp_t) default_args;
		}
		
		// Print each file in turn.  Return whether any errors occurred.
		int exit_status = EXIT_SUCCESS;
		
		while ( *args != NULL )
		{
			const char* pathname = EvaluateMetaFilename( *args++ );
			
			try
			{
				NN::Owned< p7::fd_t > fd = p7::open( pathname, p7::o_rdonly );
				
				p7::pump( fd, p7::stdout_fileno );
			}
			catch ( const p7::errno_t& error )
			{
				p7::perror( argv0, pathname, error );
				
				exit_status = EXIT_FAILURE;
				
				continue;
			}
		}
		
		return exit_status;
	}
	
}

