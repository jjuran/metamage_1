/*
	signals.cc
	----------
*/

// Standard C
#include <signal.h>
#include <stdlib.h>

// POSIX
#include <unistd.h>
#include <sys/socket.h>
#include <sys/wait.h>

// Relix
#include "relix/sched.h"

// iota
#include "iota/strings.hh"

// tap-out
#include "tap/check.hh"
#include "tap/test.hh"


static const unsigned n_tests = 1 * 3 + 2 + 1 + 3;


using tap::ok_if;


#ifndef __LAMP__
#define vfork() fork()
#endif

static pid_t clone( int (*f)(void*), void* param )
{
#ifdef __LAMP__
	
	return _relix_clone( f, NULL, 0, CLONE_VM, param );
	
#else
	
	pid_t child = CHECK( fork() );
	
	if ( child == 0 )
	{
		_exit( f( param ) );
	}
	
	return child;
	
#endif
}

static volatile sig_atomic_t global_count = 0;

static void increment( int )
{
	++global_count;
}

static void not_handler( int )
{
	CHECK( write( 1, STR_LEN( "not " ) ) );
}

static void block_signal( int signum )
{
	sigset_t block_one;
	
	CHECK( sigemptyset( &block_one ) );
	
	CHECK( sigaddset( &block_one, signum ) );
	
	CHECK( sigprocmask( SIG_BLOCK, &block_one, NULL ) );
}

static void unblock_signals()
{
	sigset_t empty;
	
	CHECK( sigemptyset( &empty ) );
	
	CHECK( sigprocmask( SIG_SETMASK, &empty, NULL ) );
}

static bool is_pending( int signum )
{
	sigset_t pending;
	
	CHECK( sigpending( &pending ) );
	
	return CHECK( sigismember( &pending, signum ) );
}


static void block_and_raise_sigusr1()
{
	const pid_t pid = CHECK( vfork() );
	
	if ( pid == 0 )
	{
		block_signal( SIGUSR1 );
		
		// SIGUSR1 is blocked, so it shouldn't kill us
		
		CHECK( raise( SIGUSR1 ) );
		CHECK( raise( SIGKILL ) );
	}
	
	int wait_status;
	
	CHECK( wait( &wait_status ) );
	
	ok_if( wait_status == SIGKILL );
}

static void block_and_raise_sigusr1_not()
{
	const pid_t pid = CHECK( vfork() );
	
	if ( pid == 0 )
	{
		block_signal( SIGUSR1 );
		
		CHECK( (int) signal( SIGUSR1, &not_handler ) );
		
		// SIGUSR1 is blocked, so not_handler() shouldn't run (yet)
		
		CHECK( raise( SIGUSR1 ) );  // writes "not " if not blocked
		CHECK( raise( SIGKILL ) );  // make sure we die without invoking usr1
	}
	
	int wait_status;
	
	CHECK( wait( &wait_status ) );
	
	ok_if( true );
}

static void block_and_raise_sigkill()
{
	const pid_t pid = CHECK( vfork() );
	
	if ( pid == 0 )
	{
		block_signal( SIGKILL );
		
		CHECK( raise( SIGKILL ) );
		
		// SIGKILL should not be blockable; if we get here the test failed
		
		CHECK( write( 1, STR_LEN( "not " ) ) );
		
		_exit( 1 );
	}
	
	int wait_status;
	
	CHECK( wait( &wait_status ) );
	
	ok_if( true );
}

static void discarded_pending()
{
	block_signal( SIGWINCH );
	
	CHECK( raise( SIGWINCH ) );
	
	ok_if( is_pending( SIGWINCH ) );
	
	CHECK( (int) signal( SIGWINCH, SIG_IGN ) );
	
	ok_if( !is_pending( SIGWINCH ) );
}

static void ignored_not_pending()
{
	CHECK( (int) signal( SIGUSR1, SIG_IGN ) );
	
	block_signal( SIGUSR1 );
	
	CHECK( raise( SIGUSR1 ) );
	
	ok_if( !is_pending( SIGUSR1 ) );
}

static ssize_t n_read = -123;

static int read_fd( void* fds_ )
{
	int* fds = (int*) fds_;
	
	CHECK( close( fds[1] ) );
	
	const struct sigaction action = { increment, sigset_t(), SA_RESTART };
	
	CHECK( sigaction( SIGUSR1, &action, NULL ) );
	
	char buffer[ 8 ];
	
	n_read = read( fds[0], buffer, sizeof buffer );
	
	CHECK( write( fds[0], n_read < 0 ? "-" : "+", 1 ) );
	
	return n_read < 0;
}

static void restart()
{
	unblock_signals();
	
	int pair[2];
	
	CHECK( socketpair( PF_LOCAL, SOCK_STREAM, 0, pair ) );
	
	const pid_t child_pid = CHECK( clone( read_fd, pair ) );
	
	CHECK( write( pair[0], "0", 1 ) );
	
	sleep( 0 );
	
	CHECK( kill( child_pid, SIGUSR1 ) );
	
	sleep( 0 );
	
	char buffer[ 8 ];
	
	ssize_t n_read = CHECK( read( pair[1], buffer, sizeof buffer ) );
	
	ok_if( n_read == 1 );
	
	CHECK( shutdown( pair[1], SHUT_WR ) );
	
	sleep( 0 );
	
	n_read = CHECK( read( pair[1], buffer, 1 ) );
	
	ok_if( n_read == 1 );
	
	ok_if( buffer[0] == '+' );
	
	int wait_status = -1;
	
	CHECK( waitpid( child_pid, &wait_status, 0 ) );
}


int main( int argc, char** argv )
{
	tap::start( "signals", n_tests );
	
	block_and_raise_sigusr1();
	block_and_raise_sigusr1_not();
	block_and_raise_sigkill();
	
	discarded_pending();
	ignored_not_pending();
	
	restart();
	
	return 0;
}

