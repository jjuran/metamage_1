/*	==========
 *	killall.cc
 *	==========
 */

// Standard C
#include <errno.h>
#include <signal.h>
#include <string.h>

// Standard C/C++
#include <cctype>
#include <cstdlib>

// POSIX
#include <dirent.h>
#include <fcntl.h>
#include <unistd.h>

// Extended API Set Part 2
#include "extended-api-set/part-2.h"

// Traditional
#include <alloca.h>

// iota
#include "iota/decimal.hh"

// more-posix
#include "more/perror.hh"

// klibc
#include "klibc/signal_lookup.hh"

// Orion
#include "Orion/Main.hh"


#ifndef O_BINARY
#define O_BINARY 0
#endif


namespace tool
{
	
	static int killall( const char* name_to_kill, int sig )
	{
		const size_t name_len = strlen( name_to_kill );
		
		const size_t buffer_size = name_len + 1;
		
		char* buffer = (char*) alloca( buffer_size );
		
		const char* proc_dir = "/proc";
		
		DIR* dir = opendir( proc_dir );
		
		if ( dir == NULL )  return -1;
		
		const int proc_dirfd = dirfd( dir );
		
		int kills = 0;
		
		while ( dirent* entry = readdir( dir ) )
		{
			const char* proc_id = entry->d_name;
			
			if ( pid_t pid = iota::parse_unsigned_decimal( proc_id ) )
			{
				int pid_dirfd = openat( proc_dirfd, proc_id, O_RDONLY | O_DIRECTORY );
				
				int name_fd = openat( pid_dirfd, "name", O_RDONLY | O_BINARY );
				
				const ssize_t n_read = read( name_fd, buffer, buffer_size );
				
				if ( n_read == name_len  &&  memcmp( name_to_kill, buffer, name_len ) == 0 )
				{
					const int killed = kill( pid, sig );
					
					if ( killed == 0 )
					{
						++kills;
					}
					else
					{
						more::perror( name_to_kill, iota::inscribe_decimal( pid ) );
					}
				}
				
				close( name_fd   );
				close( pid_dirfd );
			}
		}
		
		closedir( dir );
		
		return kills;
	}
	
	
	int Main( int argc, iota::argv_t argv )
	{
		int sig_number = SIGTERM;
		
		char const *const *argp = argv;
		
		if ( argc > 1  &&  argp[ 1 ][ 0 ] == '-' )
		{
			const char* sig = argp[ 1 ] + 1;
			
			bool numeric = std::isdigit( *sig );
			
			// FIXME:  Needs error checking instead of silently using 0
			sig_number = numeric ? iota::parse_unsigned_decimal( sig )
			                     : klibc::signal_lookup        ( sig );
			
			++argp;
			--argc;
		}
		
		if ( argc != 2 )
		{
			const char usage[] = "killall: usage: killall [-sig] name\n";
			
			(void) write( STDERR_FILENO, usage, sizeof usage - 1 );
			
			return EXIT_FAILURE;
		}
		
		int kills = killall( argp[ 1 ], sig_number );
		
		if ( kills == 0 )
		{
			more::perror( argp[1], "no process killed", 0 );
			
			return EXIT_FAILURE;
		}
		
		return kills > 0 ? EXIT_SUCCESS : EXIT_FAILURE;
	}
	
}

