/*
	signals.cc
	----------
*/

// Standard C
#include <signal.h>
#include <stdlib.h>

// POSIX
#include <unistd.h>
#include <sys/wait.h>

// iota
#include "iota/strings.hh"

// tap-out
#include "tap/check.hh"
#include "tap/test.hh"


static const unsigned n_tests = 1 * 3 + 2 + 1;


using tap::ok_if;


#ifndef __LAMP__
#define vfork() fork()
#endif


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


int main( int argc, char** argv )
{
	tap::start( "signals", n_tests );
	
	block_and_raise_sigusr1();
	block_and_raise_sigusr1_not();
	block_and_raise_sigkill();
	
	discarded_pending();
	ignored_not_pending();
	
	return 0;
}

