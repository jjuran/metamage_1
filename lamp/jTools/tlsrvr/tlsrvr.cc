/*	=========
 *	tlsrvr.cc
 *	=========
 */

// Standard C
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// command
#include "command/get_option.hh"

// gear
#include "gear/inscribe_decimal.hh"

// mac-sys-utils
#include "mac_sys/errno_from_mac_error.hh"

// plus
#include "plus/mac_utf8.hh"
#include "plus/string/concat.hh"
#include "plus/var_string.hh"

// poseven
#include "poseven/functions/perror.hh"
#include "poseven/types/errno_t.hh"

// Nitrogen
#include "Mac/Toolbox/Types/OSStatus.hh"

// Orion
#include "Orion/Main.hh"

// tlsrvr
#include "escape.hh"
#include "RunToolServer.hh"


#define UTF8_ZERO_WIDTH_SPACE  "\xe2\x80\x8b"
#define UTF8_PILCROW           "\xc2\xb6"

#define PATH_MARKER  UTF8_ZERO_WIDTH_SPACE UTF8_PILCROW

#define STRLEN( s )  (sizeof "" s - 1)
#define STR_LEN( s )  s, STRLEN( s )


using namespace command::constants;

enum
{
	Option_last_byte = 255,
	
	Option_escape,
	Option_switch,
};

static command::option options[] =
{
	{ "escape", Option_escape },
	{ "switch", Option_switch },
	
	{ NULL }
};

static bool escapeForMPW = false;
static bool switchLayers = false;

static char* const* get_options( char* const* argv )
{
	++argv;  // skip arg 0
	
	short opt;
	
	while ( (opt = command::get_option( &argv, options )) )
	{
		switch ( opt )
		{
			case Option_escape:
				escapeForMPW = true;
				break;
			
			case Option_switch:
				switchLayers = true;
				break;
			
			default:
				abort();
		}
	}
	
	return argv;
}


namespace tool
{
	
	namespace p7 = poseven;
	
	
	static inline
	bool begins_with( const plus::string& word, const char* token, int len )
	{
		return word.substr( 0, len ) == token;
	}
	
	static plus::string MakeCommand( char const *const *begin, char const *const *end, bool needToEscape )
	{
		plus::var_string command;
		
		for ( char const *const *it = begin;  it < end;  )
		{
			const char* arg = *it++;
			
			plus::string word( arg, strlen( arg ), vxo::delete_never );
			
			if ( begins_with( word, STR_LEN( PATH_MARKER ) )  &&  it < end )
			{
				word = plus::concat( word.substr( STRLEN( PATH_MARKER ) ),
				                     escaped_HFS_path( *it++ ) );
			}
			else
			{
				word = plus::mac_from_utf8( word );
				
				if ( needToEscape )
				{
					word = escaped( word );
				}
			}
			
			command += word;
			
			command += " ";
		}
		
		if ( !command.empty() )
		{
			command.resize( command.size() - 1 );
		}
		
		return command;
	}
	
	int Main( int argc, char** argv )
	{
		char *const *args = get_options( argv );
		
		const int argn = argc - (args - argv);
		
		if ( const char* frontmost = getenv( "TLSRVR_FRONTMOST" ) )
		{
			if ( strcmp( frontmost, "always" ) == 0 )
			{
				switchLayers = true;
			}
			else if ( strcmp( frontmost, "never" ) == 0 )
			{
				// do nothing
			}
			else
			{
				fprintf( stderr, "TLSRVR_FRONTMOST must be always|never, not %s\n", frontmost );
				
				return 2;
			}
		}
		
		plus::string command = MakeCommand( args,
		                                    args + argn,
		                                    escapeForMPW );
		
		try
		{
			return RunCommandInToolServer( command, switchLayers );
		}
		catch ( const Mac::OSStatus& err )
		{
			plus::var_string status = "OSStatus ";
			
			status += gear::inscribe_decimal( err );
			
			p7::perror( "tlsrvr", status, 0 );
			
			int result = mac::sys::errno_from_mac_error( err );
			
			if ( result > 0 )
			{
				p7::throw_errno( result );
			}
			
			throw;
		}
		
		// Not reached
		return 0;
	}

}
