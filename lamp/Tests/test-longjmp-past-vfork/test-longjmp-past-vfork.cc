/*	==========================
 *	test-longjmp-past-vfork.cc
 *	==========================
 */

// POSIX
#include <errno.h>
#include <setjmp.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>

// Iota
#include "iota/strings.hh"


static jmp_buf global_jmpbuf;

static void control_sigsegv_handler_1( int signo )
{
	write( STDOUT_FILENO, STR_LEN( "not ok 1\n" ) );
	
	_exit( 0 );
}

static void control_sigsegv_handler_2( int signo )
{
	write( STDOUT_FILENO, STR_LEN( "not ok 2\n" ) );
	
	_exit( 0 );
}

static void experimental_sigsegv_handler( int signo )
{
	write( STDOUT_FILENO, STR_LEN( "ok 3\n" ) );
	
	_exit( 0 );
}

static void control_1()
{
	pid_t pid = vfork();
	
	if ( pid == 0 )
	{
		_exit( 0 );
	}
	
	write( STDOUT_FILENO, STR_LEN( "ok 1\n" ) );
}

static void control_2()
{
	pid_t pid = vfork();
	
	if ( pid == 0 )
	{
		kill( getpid(), SIGKILL );
	}
	
	write( STDOUT_FILENO, STR_LEN( "ok 2\n" ) );
}

static void experiment()
{
	static bool shared_memory = false;
	
	pid_t pid = vfork();
	
	if ( pid == 0 )
	{
		// This doesn't affect the parent if vfork() is really fork().
		shared_memory = true;
		
		longjmp( global_jmpbuf, 1 );
	}
	
	// If memory is shared between parent and child, then we have a 'real'
	// vfork().  In this case, when vfork() returns in the parent, the stack is
	// already invalid.
	// What we're testing is that the kernel won't let this happen.
	// If we get here, we failed, so report it and get the hell out.
	
	if ( shared_memory )
	{
		write( STDOUT_FILENO, STR_LEN( "not " ) );
	}
	
	// Otherwise, vfork() is really fork() and there's no problem.
	
	write( STDOUT_FILENO, STR_LEN( "ok 3\n" ) );
	
	_exit( 0 );
}

int main( int argc, const char *const *argv )
{
	write( STDOUT_FILENO, STR_LEN( "1..3\n" ) );
	
	signal( SIGSEGV, &control_sigsegv_handler_1 );
	
	control_1();
	
	signal( SIGSEGV, &control_sigsegv_handler_2 );
	
	control_2();
	
	signal( SIGSEGV, &experimental_sigsegv_handler );
	
	if ( setjmp( global_jmpbuf ) == 0 )
	{
		experiment();
	}
	else
	{
		_exit( 1 );
	}
	
	// Not reached
	return 0;
}

