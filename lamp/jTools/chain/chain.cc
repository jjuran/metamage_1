/*	========
 *	chain.cc
 *	========
 */

// POSIX
#include <sys/wait.h>
#include <unistd.h>

// more-posix
#include "more/perror.hh"


#pragma exceptions off


enum link_type
{
	link_none,  // not a link
	link_then,  // ';'
	link_end    // trailing NULL
};

static link_type get_link_type( const char*& arg )
{
	if ( arg == NULL )
	{
		return link_end;
	}
	
	if ( arg[0] == ';' && arg[1] == '\0' )
	{
		return link_then;
	}
	
	if ( arg[0] == '\\' )
	{
		++arg;
	}
	
	return link_none;
}

int main( int argc, const char *argv[] )
{
	const char **argq = argv;  // skip argv[0] later
	
	link_type type;
	
	do
	{
		const char **argp = argq + 1;
		
		while ( (type = get_link_type( *++argq )) == link_none )
		{
			continue;
		}
		
		if ( argp == argq )
		{
			continue;
		}
		
		*argq = NULL;  // null out the separator
		
		// Don't fork if we're last
		pid_t pid = type == link_end ? 0 : vfork();
		
		if ( pid < 0 )
		{
			more::perror( "chain: vfork" );
			
			return 1;
		}
		
		// child process, or end of chain
		if ( pid == 0 )
		{
			(void) execvp( argp[ 0 ], (char**) argp );
			
			const int exit_code = errno == ENOENT ? 127 : 126;
			
			more::perror( argv[0], argp[0] );
			
			_exit( exit_code );
		}
		
		int wait_status = -1;
		
		wait( &wait_status );
	}
	while ( type != link_end );
	
	return 0;
}

