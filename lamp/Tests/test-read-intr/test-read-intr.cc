/*	=================
 *	test-read-intr.cc
 *	=================
 */

// POSIX
#include <errno.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>

// Iota
#include "iota/strings.hh"


volatile sig_atomic_t cont_flag = 0;
volatile sig_atomic_t alrm_flag = 0;

static void sigcont_handler( int signo )
{
	cont_flag = 1;
}

static void sigalrm_handler( int signo )
{
	alrm_flag = 1;
}

int main( int argc, const char *const *argv )
{
	struct sigaction new_action = { &sigcont_handler, sigset_t(), 0 };
	
	sigaction( SIGCONT, &new_action, NULL );
	
	new_action.sa_handler = &sigalrm_handler;
	
	sigaction( SIGALRM, &new_action, NULL );
	
	write( STDOUT_FILENO, STR_LEN( "1..3\n" ) );
	
	const char* chain[] = { "chain", "sleep", "0.1", ";", "kill", "-CONT", "0", NULL };
	
	pid_t pid = vfork();
	
	if ( pid == 0 )
	{
		execvp( chain[0], (char**) &chain[0] );
		
		_exit( errno == ENOENT ? 127 : 126 );
	}
	
	int pipe_ends[2];
	
	pipe( pipe_ends );
	
	char buffer[ 4096 ];
	
	alarm( 1 );
	
	ssize_t bytes_read = read( pipe_ends[0], buffer, 4096 );
	
	int e = errno;
	
	alarm( 0 );
	
	int wait_status = -1;
	
	wait( &wait_status );
	
	if ( !cont_flag )
	{
		write( STDOUT_FILENO, STR_LEN( "not " ) );
	}
	
	write( STDOUT_FILENO, STR_LEN( "ok 1\n" ) );
	
	if ( alrm_flag )
	{
		write( STDOUT_FILENO, STR_LEN( "not " ) );
	}
	
	write( STDOUT_FILENO, STR_LEN( "ok 2\n" ) );
	
	if ( !( bytes_read == -1  &&  e == EINTR ) )
	{
		write( STDOUT_FILENO, STR_LEN( "not " ) );
	}
	
	write( STDOUT_FILENO, STR_LEN( "ok 3\n" ) );
	
	return 0;
}

