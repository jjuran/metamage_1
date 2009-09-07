/*	========
 *	nohup.cc
 *	========
 */

// Standard C
#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

// Standard C++
#include <string>

// POSIX
#include <fcntl.h>
#include <unistd.h>

// Iota
#include "iota/strings.hh"

// POSeven
#include "POSeven/functions/perror.hh"


namespace p7 = poseven;


int main( int argc, char *const argv[] )
{
	if ( argc < 2 )
	{
		(void) write( STDERR_FILENO, STR_LEN( "Usage: nohup command [ arg1 ... argn ]\n" ) );
		
		return 1;
	}
	
	signal( SIGHUP, SIG_IGN );
	
	if ( isatty( STDOUT_FILENO ) )
	{
		std::string nohup_out = "nohup.out";
		
		int output = open( nohup_out.c_str(), O_WRONLY | O_CREAT | O_APPEND, 0600 );
		
		if ( output == -1 )
		{
			if ( const char* home = getenv( "HOME" ) )
			{
				nohup_out = home;
				
				nohup_out += "/nohup.out";
				
				output = open( nohup_out.c_str(), O_WRONLY | O_CREAT | O_APPEND, 0600 );
			}
			
			if ( output == -1 )
			{
				p7::perror( "nohup: can't open a nohup.out file." );
				
				return 127;
			}
		}
		
		std::fprintf( stderr, "sending output to %s\n", nohup_out.c_str() );
		
		dup2( output, STDOUT_FILENO );
		
		close( output );
	}
	
	if ( isatty( STDERR_FILENO ) )
	{
		dup2( STDOUT_FILENO, STDERR_FILENO );
	}
	
	(void) execvp( argv[ 1 ], argv + 1 );
	
	bool noSuchFile = errno == ENOENT;
	
	p7::perror( argv[0], argv[1] );
	
	return noSuchFile ? 127 : 126;
}

