/*	=======
 *	time.cc
 *	=======
 */

// POSIX
#include <sys/time.h>
#include <sys/times.h>
#include <sys/wait.h>
#include <unistd.h>

// Standard C/C++
#include <cstdio>

// Standard C
#include <errno.h>
#include <stdlib.h>
#include <time.h>

// Orion
#include "Orion/Main.hh"


static inline
int exit_from_wait( int status )
{
	int result = WIFEXITED  ( status ) ? WEXITSTATUS( status )
	           : WIFSIGNALED( status ) ? WTERMSIG   ( status ) + 128
	           :                         -1;
	
	return result;
}

namespace tool
{
	
	int Main( int argc, char** argv )
	{
		struct timeval tv_a = { 0 };
		struct timeval tv_b = { 0 };
		
		struct tms tms_a = { 0 };
		struct tms tms_b = { 0 };
		
		times( &tms_a );
		
		gettimeofday( &tv_a, NULL );
		
		pid_t pid = vfork();
		
		if ( pid < 0 )
		{
			_exit( 255 );
		}
		
		if ( pid == 0 )
		{
			if ( argc <= 1 )
			{
				_exit( 0 );
			}
			
			++argv;
			
			execvp( argv[0], argv );
			
			_exit( errno == ENOENT ? 127 : 126 );
		}
		
		int wait_status = -1;
		wait( &wait_status );
		
		gettimeofday( &tv_b, NULL );
		
		times( &tms_b );
		
		unsigned long long a = tv_a.tv_sec * 1000000ull + tv_a.tv_usec;
		unsigned long long b = tv_b.tv_sec * 1000000ull + tv_b.tv_usec;
		
		unsigned long long real_diff = b - a;
		unsigned long long user_diff = tms_b.tms_cutime - tms_a.tms_cutime;
		unsigned long long sys_diff  = tms_b.tms_cstime - tms_a.tms_cstime;
		
		unsigned real_int = real_diff / 1000000;
		unsigned real_dec = real_diff / 10000 % 100;
		
		unsigned user_int = user_diff / CLOCKS_PER_SEC;
		unsigned user_dec = user_diff / (CLOCKS_PER_SEC / 100) % 100;
		
		unsigned sys_int = sys_diff / CLOCKS_PER_SEC;
		unsigned sys_dec = sys_diff / (CLOCKS_PER_SEC / 100) % 100;
		
		std::fprintf( stderr,             "\n"
		                      "real %u.%.2u" "\n"
		                      "user %u.%.2u" "\n"
		                      "sys  %u.%.2u" "\n",
		                      real_int, real_dec,
		                      user_int, user_dec,
		                      sys_int,  sys_dec );
		
		return exit_from_wait( wait_status );
	}

}
