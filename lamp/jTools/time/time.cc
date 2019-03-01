/*	=======
 *	time.cc
 *	=======
 */

// Standard C/C++
#include <cstdio>

// Standard C
#include <time.h>

// poseven
#include "poseven/functions/execvp.hh"
#include "poseven/functions/gettimeofday.hh"
#include "poseven/functions/times.hh"
#include "poseven/functions/vfork.hh"
#include "poseven/functions/wait.hh"
#include "poseven/functions/_exit.hh"

// Orion
#include "Orion/Main.hh"


namespace tool
{
	
	namespace n = nucleus;
	namespace p7 = poseven;
	
	
	int Main( int argc, char** argv )
	{
		struct timeval tv_a = { 0 };
		struct timeval tv_b = { 0 };
		
		struct tms tms_a = { 0 };
		struct tms tms_b = { 0 };
		
		p7::times( tms_a );
		
		p7::gettimeofday( tv_a );
		
		p7::pid_t pid = POSEVEN_VFORK();
		
		if ( pid == 0 )
		{
			if ( argc <= 1 )
			{
				p7::_exit( p7::exit_success );
			}
			
			++argv;
			
			p7::execvp( argv[0], argv );
		}
		
		p7::wait_t wait_status = p7::wait();
		
		p7::gettimeofday( tv_b );
		
		p7::times( tms_b );
		
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
		
		return n::convert< p7::exit_t >( wait_status );
	}

}
