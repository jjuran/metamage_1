/*
	thds.cc
	-------
*/

// POSIX
#include <unistd.h>

// Standard C
#include <string.h>

// mac-sys-utils
#include "mac_sys/default_thread_stack_size.hh"
#include "mac_sys/has/ThreadManager.hh"

// gear
#include "gear/inscribe_decimal.hh"


#define DEFAULT_THREAD_STACK_SIZE "Default thread stack size"

#define STR_LEN( s )  "" s, (sizeof s - 1)


static inline
bool wrote( int fd, const void* buffer, size_t n )
{
	return write( fd, buffer, n ) == n;
}

int main( int argc, char** argv )
{
	using namespace mac::sys;
	
	if ( has_ThreadManager() )
	{
		if ( long size = default_thread_stack_size() )
		{
			char* decimal = gear::inscribe_unsigned_decimal( size );
			
			size_t len = strlen( decimal );
			
			decimal[ len++ ] = '\n';
			
			const int fd = STDOUT_FILENO;
			
			int ok = wrote( fd, STR_LEN( DEFAULT_THREAD_STACK_SIZE ": " ) )  &&
			         wrote( fd, decimal, len );
			
			return ok ? 0 : 13;
		}
		
		return 1;
	}
	
	return 2;
}
