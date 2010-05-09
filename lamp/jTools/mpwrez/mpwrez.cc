/*	=========
 *	mpwrez.cc
 *	=========
 */

// Standard C++
#include <list>
#include <vector>

// plus
#include "plus/var_string.hh"

// poseven
#include "poseven/functions/execvp.hh"
#include "poseven/functions/vfork.hh"
#include "poseven/functions/wait.hh"
#include "poseven/functions/write.hh"

// GetPathname
#include "GetPathname.hh"

// Divergence
#include "Divergence/Utilities.hh"

// Orion
#include "Orion/Main.hh"


namespace tool
{
	
	namespace p7 = poseven;
	namespace Div = Divergence;
	
	
	template < class Iter >
	plus::string join( const plus::string& glue, Iter begin, Iter end )
	{
		if ( begin == end )
		{
			return "";
		}
		
		plus::var_string result = *begin++;
		
		while ( begin != end )
		{
			result += glue;
			result += *begin++;
		}
		
		return result;
	}
	
	
	static plus::string MacPathFromPOSIXPath( const char* pathname )
	{
		FSSpec item = Div::ResolvePathToFSSpec( pathname );
		
		return GetMacPathname( item );
	}
	
	
	static const char* StoreMacPathFromPOSIXPath( const char* pathname )
	{
		static std::list< plus::string > static_string_storage;
		
		static_string_storage.push_back( MacPathFromPOSIXPath( pathname ) );
		
		return static_string_storage.back().c_str();
	}
	
	
	int Main( int argc, iota::argv_t argv )
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
			plus::var_string output = join( " ", command.begin(), command.end() );
			
			output += '\n';
			
			p7::write( p7::stdout_fileno, output );
		}
		
		command.push_back( NULL );
		
		if ( dry_run )
		{
			return p7::exit_success;
		}
		
		p7::pid_t pid = POSEVEN_VFORK();
		
		if ( pid == 0 )
		{
			p7::execvp( &command[0] );
		}
		
		return nucleus::convert< p7::exit_t >( p7::wait() );
	}
	
}

