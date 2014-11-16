/*
	coprocess.cc
	------------
*/

#include "poseven/extras/coprocess.hh"

// POSIX
#include <signal.h>
#include <sys/wait.h>

// debug
#include "debug/assert.hh"

// poseven
#include "poseven/functions/waitpid.hh"
#include "poseven/types/errno_t.hh"


namespace poseven
{
	
	void coprocess::kill( int signum ) const
	{
		ASSERT( its_pid != 0 );
		
		throw_posix_result( ::kill( its_pid, signum ) );
	}
	
	wait_t coprocess::wait()
	{
		ASSERT( its_pid != 0 );
		
		const wait_t result = waitpid( its_pid );
		
		its_pid = pid_t();
		
		return result;
	}
	
	wait_t coprocess::terminate()
	{
		ASSERT( its_pid != 0 );
		
		kill( SIGTERM );
		
		return wait();
	}
	
}
