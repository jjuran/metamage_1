/*	==========
 *	killall.cc
 *	==========
 */

// Standard C
#include <errno.h>
#include <signal.h>
#include <stdio.h>

// Standard C/C++
#include <cctype>
#include <cstdlib>
#include <cstring>

// Standard C++
#include <string>

// POSIX
#include <dirent.h>
#include <unistd.h>

// iota
#include "iota/decimal.hh"

// klibc
#include "klibc/signal_lookup.hh"

// Orion
#include "Orion/Main.hh"


namespace tool
{
	
	static bool eq_strings( const char* a, const char* b )
	{
		return std::strcmp( a, b ) == 0;
	}
	
	static void kill_process( pid_t pid, int sig )
	{
		int killed = kill( pid, sig );
	}
	
	static std::string read_link( const char* link_path )
	{
		const std::size_t path_len = 1024;
		
		char target_path[ path_len + 1];
		
		int result = readlink( link_path, target_path, path_len );
		
		if ( result == -1 )
		{
			return "";
		}
		
		return std::string( target_path, result );
	}
	
	static int killall( const char* name_to_kill, int sig )
	{
		const char* proc_dir = "/proc";
		
		DIR* dir = opendir( proc_dir );
		
		if ( dir == NULL )  return -1;
		
		int kills = 0;
		
		while ( dirent* entry = readdir( dir ) )
		{
			const char* proc_id = entry->d_name;
			
			if ( pid_t pid = iota::parse_unsigned_decimal( proc_id ) )
			{
				std::string exe = std::string( "/proc/" ) + proc_id + "/exe";
				
				std::string target_path = read_link( exe.c_str() );
				
				if ( const char* proc_name = std::strrchr( target_path.c_str(), '/' ) )
				{
					if ( eq_strings( ++proc_name, name_to_kill ) )
					{
						int killed = kill( pid, sig );
						
						if ( killed == 0 )
						{
							++kills;
						}
						else
						{
							std::fprintf( stderr, "%s(%d): %s\n", proc_name, pid, std::strerror( errno ) );
						}
					}
				}
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
			std::fprintf( stderr, "%s: %s\n", argp[1], "no process killed" );
			
			return EXIT_FAILURE;
		}
		
		return kills > 0 ? EXIT_SUCCESS : EXIT_FAILURE;
	}
	
}

