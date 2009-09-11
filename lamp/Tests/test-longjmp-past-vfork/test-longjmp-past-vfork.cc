/*	==========================
 *	test-longjmp-past-vfork.cc
 *	==========================
 */

// POSIX
#include <errno.h>
#include <setjmp.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/wait.h>

// Iota
#include "iota/strings.hh"


#ifndef SIGSTKFLT
// SIGSTKFLT is not defined, so we're not in Lamp.
// Define it to something harmless so we can run the tests anyway.
#define SIGSTKFLT SIGALRM
#endif

static jmp_buf global_main_jmpbuf;
static jmp_buf global_test_jmpbuf;

static bool fatal_subtest = false;

static void control_sigstkflt_handler_1( int signo )
{
	// Exiting within the scope of the vfork should not cause a stack fault.
	write( STDOUT_FILENO, STR_LEN( "not ok 1\n" ) );
	
	_exit( 0 );
}

static void control_sigstkflt_handler_2( int signo )
{
	// Dying within the scope of the vfork should not cause a stack fault.
	write( STDOUT_FILENO, STR_LEN( "not ok 2\n" ) );
	
	_exit( 0 );
}

static void experimental_sigstkflt_handler( int signo )
{
	// The stack fault was detected and SIGSTKFLT delivered.  Success!
	write( STDOUT_FILENO, STR_LEN( "ok 3\n" ) );
	
	// Now get the hell out of here.
	longjmp( global_main_jmpbuf, 3 );
}

static void control_1()
{
	pid_t pid = vfork();
	
	if ( pid == 0 )
	{
		_exit( 0 );  // should not cause a stack fault
	}
	
	int w = -1;
	
	wait( &w );
	
	write( STDOUT_FILENO, STR_LEN( "ok 1\n" ) );
}

static void control_2()
{
	pid_t pid = vfork();
	
	if ( pid == 0 )
	{
		kill( getpid(), SIGKILL );  // should not cause a stack fault
	}
	
	int w = -1;
	
	wait( &w );
	
	write( STDOUT_FILENO, STR_LEN( "ok 2\n" ) );
}

static void experiment()
{
	static bool shared_memory = false;
	
	static char ok[] = "ok 3\n";
	
	if ( fatal_subtest )
	{
		char* p = ok + sizeof "ok " - 1;
		
		*p = '4';
	}
	
	pid_t pid = vfork();
	
	if ( pid == 0 )
	{
		// This doesn't affect the parent if vfork() is really fork().
		shared_memory = true;
		
		// This unwinds the stack past the vfork above.
		longjmp( global_test_jmpbuf, 1 );
	}
	
	// If we get here, then the kernel didn't deliver a fatal signal.
	
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
	
	write( STDOUT_FILENO, ok, sizeof ok - 1 );
	
	if ( fatal_subtest )
	{
		// Test 4 failed, and we're the child.
		// Get the hell out, and return a failing wait status.
		
		_exit( 1 );
	}
	else if ( shared_memory )
	{
		// Test 3 failed.  Get back to sanity.
		longjmp( global_main_jmpbuf, 3 );
	}
	
	// vfork() is fork().  Skip test 4.
	write( STDOUT_FILENO, STR_LEN( "ok 4 # skipped\n" ) );
	
	_exit( 0 );
}

int main( int argc, const char *const *argv )
{
	// test-longjmp-past-vfork needs to spawn a process that causes a stack fault
	// so we can test that the kernel detects this and delivers a fatal signal.
	// The same program plays both roles, with --fatal indicating the faulty
	// behavior.
	
	fatal_subtest = argc >= 2  &&  strcmp( argv[1], "--fatal" ) == 0;
	
	if ( fatal_subtest )
	{
		// Don't dump backtrace to console
		chmod( "/proc/self/core", 0 );
	}
	else
	{
		// If this is not the fatal test run, everything is business as usual.
		
		write( STDOUT_FILENO, STR_LEN( "1..4\n" ) );
		
		// Test 1:  Call _exit() in child after vfork().
		
		signal( SIGSTKFLT, &control_sigstkflt_handler_1 );
		
		control_1();
		
		// Test 2:  Call kill( getpid(), SIGKILL ) in child after vfork().
		
		signal( SIGSTKFLT, &control_sigstkflt_handler_2 );
		
		control_2();
		
		// Test 3:  Cause a stack fault while catching SIGSTKFLT.
		
		signal( SIGSTKFLT, &experimental_sigstkflt_handler );
	}
	
	// Tests 3 and 4:  Cause a stack fault.
	
	if ( setjmp( global_main_jmpbuf ) == 0 )
	{
		if ( setjmp( global_test_jmpbuf ) == 0 )
		{
			experiment();
		}
		else
		{
			// The child from the vfork in experiment() longjmps to here.
			// Exiting would take us back to the vfork as the parent (but
			// with a trashed stack) if the kernel didn't check for this.
			
			_exit( 1 );
		}
	}
	else if ( !fatal_subtest )
	{
		// experimental_sigstkflt_handler() longjmps to here.
		
		// Test 4:  Cause a stack fault without catching SIGSTKFLT.
		
		pid_t pid = vfork();
		
		if ( pid == 0 )
		{
			// Spawn ourselves in --fatal mode to run the fourth test.
			
			const char* command[] = { argv[0], "--fatal", NULL };
			
			execvp( command[0], (char **) command );
			
			_exit( errno == ENOENT ? 127 : 126 );
		}
		
		// Wait for our child process to complete.
		
		int w = -1;
		
		waitpid( pid, &w, 0 );
		
		// Did the child get killed by SIGSTKFLT?
		
		const bool stkflt = WIFSIGNALED( w ) && WTERMSIG( w ) == SIGSTKFLT;
		
		if ( !stkflt )
		{
			write( STDOUT_FILENO, STR_LEN( "not " ) );
		}
		
		write( STDOUT_FILENO, STR_LEN( "ok 4\n" ) );
	}
	
	// Not reached
	return 0;
}

