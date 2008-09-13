/*	=========
 *	mpwrez.cc
 *	=========
 */

// Standard C++
#include <list>
#include <string>
#include <vector>

// POSIX
#include <sys/wait.h>

// POSeven
#include "POSeven/functions/vfork.hh"

// GetPathname
#include "GetPathname.hh"

// Divergence
#include "Divergence/Utilities.hh"

// Orion
#include "Orion/Main.hh"


namespace O = Orion;


static int exit_from_wait( int stat )
{
	int result = WIFEXITED( stat )   ? WEXITSTATUS( stat )
	           : WIFSIGNALED( stat ) ? WTERMSIG( stat ) + 128
	           :                       -1;
	
	return result;
}


namespace jTools
{
	
	namespace p7 = poseven;
	namespace Div = Divergence;
	
	
	template < class Iter >
	std::string join( Iter begin, Iter end, const std::string& glue = "" )
	{
		if ( begin == end )
		{
			return "";
		}
		
		std::string result = *begin++;
		
		while ( begin != end )
		{
			result += glue;
			result += *begin++;
		}
		
		return result;
	}
	
	
	static std::string MacPathFromPOSIXPath( const char* pathname )
	{
		FSSpec item = Div::ResolvePathToFSSpec( pathname );
		
		return GetMacPathname( item );
	}
	
	
	static const char* StoreMacPathFromPOSIXPath( const char* pathname )
	{
		static std::list< std::string > static_string_storage;
		
		static_string_storage.push_back( MacPathFromPOSIXPath( pathname ) );
		
		return static_string_storage.back().c_str();
	}
	
	
	static int Main( int argc, iota::argv_t argv )
	{
		std::vector< const char* > command;
		
		command.push_back( "tlsrvr"   );
		command.push_back( "--switch" );  // bring ToolServer to front
		command.push_back( "--escape" );  // escape arguments to prevent expansion
		command.push_back( "--"       );  // stop interpreting options here
		command.push_back( "Rez"      );
		command.push_back( "-c"       );
		command.push_back( "RSED"     );
		command.push_back( "-t"       );
		command.push_back( "rsrc"     );
		
		bool dry_run = false;
		bool verbose = false;
		
		while ( const char* arg = *++argv )
		{
			if ( arg[0] == '-' )
			{
				switch ( arg[1] )
				{
					case 'n':
						dry_run = true;
						break;
					
					case 'v':
						verbose = true;
						break;
					
					case 'i':
					case 'o':
						if ( arg[2] == '\0' )
						{
							command.push_back( arg );
							
							command.push_back( StoreMacPathFromPOSIXPath( *++argv ) );
						}
						break;
					
					default:
						break;
				}
			}
			else if ( std::strchr( arg, '/' ) )
			{
				// translate path
				command.push_back( StoreMacPathFromPOSIXPath( arg ) );
			}
			else if ( arg[0] != '\0' )
			{
				command.push_back( arg );
			}
		}
		
		if ( verbose )
		{
			std::string output = join( command.begin(), command.end(), " " );
			
			output += '\n';
			
			write( STDOUT_FILENO, output.data(), output.size() );
		}
		
		command.push_back( NULL );
		
		if ( dry_run )
		{
			return EXIT_SUCCESS;
		}
		
		pid_t pid = POSEVEN_VFORK();
		
		if ( pid == 0 )
		{
			(void) execvp( command[0], (char**) &command[0] );
			
			_exit( 127 );
		}
		
		int wait_status = -1;
		
		p7::throw_posix_result( wait( &wait_status ) );
		
		return exit_from_wait( wait_status );
	}
	
}

int O::Main( int argc, argv_t argv )
{
	return jTools::Main( argc, argv );
}

