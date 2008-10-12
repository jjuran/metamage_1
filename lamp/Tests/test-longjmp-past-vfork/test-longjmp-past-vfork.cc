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


static jmp_buf global_main_jmpbuf;
static jmp_buf global_test_jmpbuf;

static bool fatal_subtest = false;

static void control_sigstkflt_handler_1( int signo )
{
	write( STDOUT_FILENO, STR_LEN( "not ok 1\n" ) );
	
	_exit( 0 );
}

static void control_sigstkflt_handler_2( int signo )
{
	write( STDOUT_FILENO, STR_LEN( "not ok 2\n" ) );
	
	_exit( 0 );
}

static void experimental_sigstkflt_handler( int signo )
{
	write( STDOUT_FILENO, STR_LEN( "ok 3\n" ) );
	
	longjmp( global_main_jmpbuf, 3 );
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
		
		longjmp( global_test_jmpbuf, 1 );
	}
	
	if ( fatal_subtest )
	{
		_exit( 1 );
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
	fatal_subtest = argc >= 2  &&  std::strcmp( argv[1], "--fatal" ) == 0;
	
	if ( !fatal_subtest )
	{
		write( STDOUT_FILENO, STR_LEN( "1..4\n" ) );
		
		signal( SIGSTKFLT, &control_sigstkflt_handler_1 );
		
		control_1();
		
		signal( SIGSTKFLT, &control_sigstkflt_handler_2 );
		
		control_2();
		
		signal( SIGSTKFLT, &experimental_sigstkflt_handler );
	}
	
	if ( setjmp( global_main_jmpbuf ) == 0 )
	{
		if ( setjmp( global_test_jmpbuf ) == 0 )
		{
			experiment();
		}
		else
		{
			_exit( 1 );
		}
	}
	else if ( !fatal_subtest )
	{
		pid_t pid = vfork();
		
		if ( pid == 0 )
		{
			const char* command[] = { argv[0], "--fatal", NULL };
			
			execvp( command[0], (char **) command );
			
			_exit( errno == ENOENT ? 127 : 126 );
		}
		
		int w = -1;
		
		waitpid( pid, &w, 0 );
		
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

