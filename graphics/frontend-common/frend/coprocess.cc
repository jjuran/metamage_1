/*
	coprocess.cc
	------------
*/

#include "frend/coprocess.hh"

// POSIX
#include <unistd.h>
#include <sys/socket.h>
#include <sys/wait.h>

// Standard C
#include <errno.h>

// gear
#include "gear/inscribe_decimal.hh"


namespace frend
{

static const char* coprocess_argv[] =
{
	"./minivx",
	"launch-ams-app.vx",
	NULL,  // #2, raster path
	NULL,  // #3, events fd
	NULL,
};

static
void exec_or_exit( const char* const argv[] )
{
	execvp( *argv, (char**) argv );
	
	_exit( errno == ENOENT ? 127 : 126 );
}

coprocess_launch::coprocess_launch( int bindir_fd, const char* works_path )
{
	int socket_fds[ 2 ];
	
	int nok = socketpair( PF_UNIX, SOCK_STREAM, 0, socket_fds );
	
	if ( nok )
	{
		throw coprocess_failed();
	}
	
	const int other_fd = socket_fds[ 0 ];
	const int local_fd = socket_fds[ 1 ];
	
	its_pid = fork();
	
	if ( its_pid < 0 )
	{
		close( other_fd );
		close( local_fd );
		
		throw coprocess_failed();
	}
	
	if ( its_pid == 0 )
	{
		fchdir( bindir_fd );
		close ( bindir_fd );
		
		close( local_fd );
		
		const char* fd_arg = gear::inscribe_unsigned_decimal( other_fd );
		
		coprocess_argv[ 2 ] = works_path;
		coprocess_argv[ 3 ] = fd_arg;       // --events-fd param
		
		exec_or_exit( coprocess_argv );
	}
	
	close( other_fd );
	
	its_socket = local_fd;
}

coprocess_launch::~coprocess_launch()
{
	int wait_status;
	waitpid( its_pid, &wait_status, 0 );
}

}
