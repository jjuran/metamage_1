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
	"../Resources/launch-app.vx",
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

coprocess_state launch_coprocess( int bindir_fd, const char* works_path )
{
	coprocess_state state;
	
	int socket_fds[ 2 ];
	
	int nok = socketpair( PF_UNIX, SOCK_STREAM, 0, socket_fds );
	
	if ( nok )
	{
		throw coprocess_failed();
	}
	
	const int other_fd = socket_fds[ 0 ];
	const int local_fd = socket_fds[ 1 ];
	
	pid_t pid = fork();
	
	if ( pid < 0 )
	{
		close( other_fd );
		close( local_fd );
		
		throw coprocess_failed();
	}
	
	if ( pid == 0 )
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
	
	state.child_pid = pid;
	state.socket_fd = local_fd;
	
	return state;
}

int wait_for_coprocess( int pid )
{
	int wait_status;
	
	waitpid( pid, &wait_status, 0 );
	
	return wait_status;
}

}
