/*	=========
 *	tlsrvr.cc
 *	=========
 */

// Standard C/C++
#include <cctype>

// Standard C
#include <string.h>

// iota
#include "iota/decimal.hh"

// plus
#include "plus/mac_utf8.hh"
#include "plus/var_string.hh"

// poseven
#include "poseven/functions/perror.hh"

// OSErrno
#include "OSErrno/OSErrno.hh"

// Orion
#include "Orion/get_options.hh"
#include "Orion/Main.hh"

// tlsrvr
#include "ToolServer.hh"
#include "RunToolServer.hh"


namespace tool
{
	
	namespace N = Nitrogen;
	namespace p7 = poseven;
	namespace o = orion;
	
	
	static plus::string QuoteForMPW( const plus::string& str )
	{
		const char* p = str.c_str();
		const char* q = p;
		
		bool needsQuoting = false;
		
		plus::var_string result = "'";
		
		while ( *p != '\0' )
		{
			while ( *q != '\0'  &&  *q != '\'' )
			{
				needsQuoting = needsQuoting || !std::isalnum( *q );
				++q;
			}
			
			result.append( p, q );
			
			if ( *q != '\0' )
			{
				needsQuoting = true;
				result += sEscapedQuote;
				++q;
			}
			
			p = q;
		}
		
		if ( !needsQuoting )
		{
			return str;
		}
		
		result += "'";
		
		return result;
	}
	
	
	static plus::string MakeCommand( char const *const *begin, char const *const *end, bool needToEscape )
	{
		plus::var_string command;
		
		for ( char const *const *it = begin;  it < end;  ++it )
		{
			plus::string word = plus::string( *it,
			                                  strlen( *it ),
			                                  plus::delete_never );
			
			word = plus::mac_from_utf8( word );
			
			command += needToEscape ? QuoteForMPW( word ) : word;
			
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
		bool escapeForMPW = false;
		bool switchLayers = false;
		
		o::bind_option_to_variable( "--escape", escapeForMPW );
		o::bind_option_to_variable( "--switch", switchLayers );
		
		o::get_options( argc, argv );
		
		char const *const *free_args = o::free_arguments();
		
		plus::string command = MakeCommand( free_args,
		                                    free_args + o::free_argument_count(),
		                                    escapeForMPW );
		
		try
		{
			return RunCommandInToolServer( command, switchLayers );
		}
		catch ( const N::OSStatus& err )
		{
			plus::var_string status = "OSStatus ";
			
			status += iota::inscribe_decimal( err );
			
			p7::perror( "tlsrvr", status, 0 );
			
			p7::throw_errno( OSErrno::ErrnoFromOSStatus( err ) );
		}
		
		// Not reached
		return 0;
	}

}

