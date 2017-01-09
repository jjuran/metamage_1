/*
	overwrite.cc
	------------
*/

// poseven
#include "poseven/extras/pump.hh"
#include "poseven/functions/fstat.hh"
#include "poseven/functions/open.hh"
#include "poseven/functions/perror.hh"

// Orion
#include "Orion/Main.hh"


namespace tool
{
	
	namespace n = nucleus;
	namespace p7 = poseven;
	
	
	static const char* const program = "overwrite";
	
	
	static inline bool path_means_stdin( const char* path )
	{
		return    path[0] == '-'
			   && path[1] == '\0';
	}
	
	static const char* get_path( const char* path )
	{
		if ( path_means_stdin( path ) )
		{
			return "/dev/fd/0";
		}
		
		return path;
	}
	
	int Main( int argc, char** argv )
	{
		const char *const * args = argv + 1;
		
		// Check for sufficient number of args
		if ( *args == NULL )
		{
			static const char *const default_args[] = { "-", NULL };
			
			args = default_args;
		}
		
		// Print each file in turn.  Return whether any errors occurred.
		int exit_status = 0;
		
		while ( *args != NULL )
		{
			const char* path = get_path( *args++ );
			
			try
			{
				n::owned< p7::fd_t > fd = p7::open( path, p7::o_wronly );
				
				struct stat sb = p7::fstat( fd );
				
				if ( sb.st_size == 0 )
				{
					continue;
				}
				
				ssize_t n_pumped = p7::pump( p7::stdin_fileno, fd, NULL, sb.st_size );
				
				if ( n_pumped < sb.st_size )
				{
					p7::perror( program, path, "input exhausted" );
					
					exit_status = 3;
				}
			}
			catch ( const p7::errno_t& error )
			{
				p7::perror( program, path, error );
				
				exit_status = 1;
				
				continue;
			}
		}
		
		return exit_status;
	}
	
}
