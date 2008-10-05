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


jmp_buf global_jmpbuf;

static void sigsegv_handler( int signo )
{
	write( STDOUT_FILENO, STR_LEN( "ok 1\n" ) );
	
	_exit( 0 );
}

static void spawn()
{
	pid_t pid = vfork();
	
	if ( pid == 0 )
	{
		longjmp( global_jmpbuf, 1 );
	}
	
	// vfork() returns in the parent, but the stack is already invalid.
	// What we're testing is that the kernel won't let this happen.
	// If we get here, we failed, so report it and get the hell out.
	
	write( STDOUT_FILENO, STR_LEN( "not ok 1\n" ) );
	
	_exit( 0 );
}

int main( int argc, const char *const *argv )
{
	signal( SIGSEGV, &sigsegv_handler );
	
	write( STDOUT_FILENO, STR_LEN( "1..1\n" ) );
	
	if ( setjmp( global_jmpbuf ) == 0 )
	{
		spawn();
	}
	else
	{
		_exit( 1 );
	}
	
	// Not reached
	return 0;
}

