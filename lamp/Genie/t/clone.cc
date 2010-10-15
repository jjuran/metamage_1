/*
	clone.cc
	--------
*/

// Standard C
#include <errno.h>
#include <signal.h>
#include <stdlib.h>

// POSIX
#include <unistd.h>
#include <sys/wait.h>

// Lamp
#include "lamp/sched.h"

// iota
#include "iota/strings.hh"

// tap-out
#include "tap/check.hh"
#include "tap/test.hh"


static const unsigned n_tests = 4 + 2 + 2 + 2 + 2;


using tap::ok_if;


static int f_zero( void* )
{
	return 0;
}

static int f_return( void* arg )
{
	return *(int*) arg;
}

static int f_exit( void* arg )
{
	_exit( *(int*) arg );
	
	return 0;
}

static int f_raise( void* arg )
{
	raise( *(int*) arg );
	
	return 0;
}

static int f_getppid( void* )
{
	return getppid();
}

static int f_clone_getppid( void* )
{
	// return the child pid
	return CHECK( _lamp_clone( &f_getppid, NULL, 0, CLONE_VM | CLONE_PARENT, NULL ) );
}

static int f_chdir( void* arg )
{
	return CHECK( chdir( (char*) arg ) );
}

static int f_close( void* arg )
{
	int fd = *(int*) arg;
	
	return CHECK( close( fd ) );
}

static int f_sigaction( void* arg )
{
	const struct sigaction dfl = { SIG_DFL };
	
	CHECK( sigaction( SIGURG, &dfl, NULL ) );
	
	return 0;
}


static int simple_clone( int (*f)( void* ), int x = 0 )
{
	int wait_status = -1;
	
	const int child_pid = CHECK( _lamp_clone( f, NULL, 0, CLONE_VM, &x ) );
	
	CHECK( waitpid( child_pid, &wait_status, 0 ) );
	
	return wait_status;
}

static void status()
{
	ok_if( simple_clone( &f_zero           ) ==       0 );
	ok_if( simple_clone( &f_return,      1 ) ==  0x0100 );
	ok_if( simple_clone( &f_exit,        1 ) ==  0x0100 );
	ok_if( simple_clone( &f_raise, SIGTERM ) == SIGTERM );
}

static void parent()
{
	const pid_t pid = CHECK( getpid() );
	
	ok_if( simple_clone( &f_getppid ) == pid << 8 );
	
	
	const int child2_pid = simple_clone( &f_clone_getppid ) >> 8;
	
	int wait_status = -1;
	
	CHECK( waitpid( child2_pid, &wait_status, 0 ) );
	
	ok_if( wait_status == pid << 8 );
}

static bool cwd_gets_changed( int more_flags )
{
	CHECK( chdir( "/" ) );
	
	int wait_status = -1;
	
	CHECK( _lamp_clone( &f_chdir, NULL, 0, CLONE_VM | more_flags, (void*) "/tmp" ) );
	
	CHECK( wait( &wait_status ) );
	
	char buffer[ 8 ] = { 0 };
	
	const char* got = getcwd( buffer, sizeof buffer );
	
	if ( !got )
	{
		abort();
	}
	
	return buffer[1] != '\0';
}

static void fs_info()
{
	ok_if(  cwd_gets_changed( CLONE_FS ) );  // share cwd
	ok_if( !cwd_gets_changed( 0        ) );  // copy cwd
}

static bool fd_gets_closed( int more_flags )
{
	int wait_status = -1;
	
	int fd_a = CHECK( dup( STDOUT_FILENO ) );
	
	CHECK( _lamp_clone( &f_close, NULL, 0, CLONE_VM | more_flags, &fd_a ) );
	
	CHECK( wait( &wait_status ) );
	
	// If the fd was closed, this one will be the same value
	int fd_b = CHECK( dup( STDOUT_FILENO ) );
	
	close( fd_a );
	close( fd_b );
	
	return fd_b == fd_a;
}

static void files()
{
	ok_if(  fd_gets_closed( CLONE_FILES ) );  // share file descriptors
	ok_if( !fd_gets_closed( 0           ) );  // copy file descriptor set
}

static bool sigaction_changed( int more_flags )
{
	int wait_status = -1;
	
	struct sigaction ignore = { SIG_IGN };
	
	CHECK( sigaction( SIGURG, &ignore, NULL ) );
	
	CHECK( _lamp_clone( &f_sigaction, NULL, 0, CLONE_VM | more_flags, NULL ) );
	
	CHECK( wait( &wait_status ) );
	
	CHECK( sigaction( SIGURG, NULL, &ignore ) );
	
	return ignore.sa_handler != SIG_IGN;
}

static void sighand()
{
	ok_if(  sigaction_changed( CLONE_SIGHAND ) );  // share signal handlers
	ok_if( !sigaction_changed( 0             ) );  // copy signal handlers
}


int main( int argc, char** argv )
{
	tap::start( "clone", n_tests );
	
	status();
	
	parent();
	
	fs_info();
	
	files();
	
	sighand();
	
	return 0;
}

