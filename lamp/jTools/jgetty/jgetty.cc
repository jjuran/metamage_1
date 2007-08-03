/*	=========
 *	jgetty.cc
 *	=========
 */

// POSIX
#include <fcntl.h>
#include <unistd.h>


#pragma export on

int main( int argc, char const *const argv[] )
{
	int console = open( "/dev/new/console", O_RDWR, 0 );
	
	if ( console == -1 )
	{
		return 1;
	}
	
	dup2( console, STDIN_FILENO  );
	dup2( console, STDOUT_FILENO );
	dup2( console, STDERR_FILENO );
	
	if ( console > STDERR_FILENO )
	{
		close( console );
	}
	
	char* exec_argv[] = { "/bin/login", NULL };
	
	int result = execv( "/bin/login", exec_argv );
	
	_exit( 127 );
	
	return 0;  // Not reached
}

#pragma export reset

