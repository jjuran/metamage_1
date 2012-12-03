/*
	clone.cc
	--------
*/

// Standard C
#include <errno.h>
#include <signal.h>
#include <stdlib.h>

// POSIX
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/wait.h>

// Relix
#include "relix/sched.h"

// iota
#include "iota/strings.hh"

// tap-out
#include "tap/check.hh"
#include "tap/test.hh"


static const unsigned n_tests = 4 + 2 + 2 + 2 + 2 + 1;


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
	return CHECK( _relix_clone( &f_getppid, NULL, 0, CLONE_VM | CLONE_PARENT, NULL ) );
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

static int f_vm( void* arg )
{
	int n = *(int*) arg;
	
	while ( n-- )
	{
		*(int*) arg = n;
		
		sched_yield();
	}
	
	return 0;
}


static int simple_clone( int (*f)( void* ), int x = 0 )
{
	int wait_status = -1;
	
	const int child_pid = CHECK( _relix_clone( f, NULL, 0, CLONE_VM, &x ) );
	
	CHECK( waitpid( child_pid, &wait_status, 0 ) );
	
	return wait_status;
}

static void status()
{
	EXPECT( simple_clone( &f_zero           ) ==       0 );
	EXPECT( simple_clone( &f_return,      1 ) ==  0x0100 );
	EXPECT( simple_clone( &f_exit,        1 ) ==  0x0100 );
	EXPECT( simple_clone( &f_raise, SIGTERM ) == SIGTERM );
}

static void parent()
{
	const pid_t pid = CHECK( getpid() );
	
	EXPECT( simple_clone( &f_getppid ) == pid << 8 );
	
	
	const int child2_pid = simple_clone( &f_clone_getppid ) >> 8;
	
	int wait_status = -1;
	
	CHECK( waitpid( child2_pid, &wait_status, 0 ) );
	
	EXPECT( wait_status == pid << 8 );
}

static bool cwd_gets_changed( int more_flags )
{
	CHECK( chdir( "/" ) );
	
	int wait_status = -1;
	
	CHECK( _relix_clone( &f_chdir, NULL, 0, CLONE_VM | more_flags, (void*) "/tmp" ) );
	
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
	EXPECT(  cwd_gets_changed( CLONE_FS ) );  // share cwd
	EXPECT( !cwd_gets_changed( 0        ) );  // copy cwd
}

static bool fd_gets_closed( int more_flags )
{
	int wait_status = -1;
	
	int fd_a = CHECK( dup( STDOUT_FILENO ) );
	
	CHECK( _relix_clone( &f_close, NULL, 0, CLONE_VM | more_flags, &fd_a ) );
	
	CHECK( wait( &wait_status ) );
	
	// If the fd was closed, this one will be the same value
	int fd_b = CHECK( dup( STDOUT_FILENO ) );
	
	close( fd_a );
	close( fd_b );
	
	return fd_b == fd_a;
}

static void files()
{
	EXPECT(  fd_gets_closed( CLONE_FILES ) );  // share file descriptors
	EXPECT( !fd_gets_closed( 0           ) );  // copy file descriptor set
}

static bool sigaction_changed( int more_flags )
{
	int wait_status = -1;
	
	struct sigaction ignore = { SIG_IGN };
	
	CHECK( sigaction( SIGURG, &ignore, NULL ) );
	
	CHECK( _relix_clone( &f_sigaction, NULL, 0, CLONE_VM | more_flags, NULL ) );
	
	CHECK( wait( &wait_status ) );
	
	CHECK( sigaction( SIGURG, NULL, &ignore ) );
	
	return ignore.sa_handler != SIG_IGN;
}

static void sighand()
{
	EXPECT(  sigaction_changed( CLONE_SIGHAND ) );  // share signal handlers
	EXPECT( !sigaction_changed( 0             ) );  // copy signal handlers
}

static void vm()
{
	int zero = CHECK( open( "/dev/zero", O_RDWR, 0 ) );
	
	const size_t size = 4096;
	
	void* memory = mmap( NULL, size, PROT_READ|PROT_WRITE, MAP_PRIVATE, zero, 0 );
	
	close( zero );
	
	CHECK( memory != NULL ? 0 : 1 );
	
	*(int*) memory = 3;
	
	CHECK( _relix_clone( &f_vm, NULL, 0, 0, memory ) );
	
	sched_yield();
	sched_yield();
	
	EXPECT( *(int*) memory == 3 );
	
	munmap( memory, size );
	
	sched_yield();
	
	int wait_status = -1;
	
	CHECK( wait( &wait_status ) );
}


int main( int argc, char** argv )
{
	tap::start( "clone", n_tests );
	
	status();
	
	parent();
	
	fs_info();
	
	files();
	
	sighand();
	
	vm();
	
	return 0;
}
