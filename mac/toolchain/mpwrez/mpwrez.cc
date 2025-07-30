/*
	mpwrez.cc
	---------
*/

// Standard C
#include <string.h>

// plus
#include "plus/var_string.hh"

// vxo
#include "vxo/ptrvec.hh"

// poseven
#include "poseven/functions/execvp.hh"
#include "poseven/functions/vfork.hh"
#include "poseven/functions/wait.hh"
#include "poseven/functions/write.hh"

// Orion
#include "Orion/Main.hh"


#define UTF8_ZERO_WIDTH_SPACE  "\xe2\x80\x8b"
#define UTF8_PILCROW           "\xc2\xb6"

#define PATH_MARKER  UTF8_ZERO_WIDTH_SPACE UTF8_PILCROW


namespace tool
{
	
	namespace p7 = poseven;
	
	
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
	
	
	int Main( int argc, char** argv )
	{
		vxo::UPtrVec< const char > command;
		
		command.push_back( "tlsrvr"   );
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
					
					case 'd':
						if ( arg[2] == '\0' )
						{
							command.push_back( arg );
							
							command.push_back( *++argv );
						}
						break;
					
					case 'i':
					case 'o':
						if ( arg[2] == '\0' )
						{
							command.push_back( arg );
							
							command.push_back( PATH_MARKER );
							command.push_back( *++argv );
						}
						break;
					
					default:
						break;
				}
			}
			else if ( strchr( arg, '/' ) )
			{
				command.push_back( PATH_MARKER );
				command.push_back( arg );
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
		
		return exit_from_wait( p7::wait() );
	}
	
}
