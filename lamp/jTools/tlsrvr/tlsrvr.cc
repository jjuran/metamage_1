/*	=========
 *	tlsrvr.cc
 *	=========
 */

// Standard C++
#include <string>

// Standard C/C++
#include <cctype>

// iota
#include "iota/decimal.hh"

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
	
	
	static std::string QuoteForMPW( const char* str )
	{
		const char* p = str;
		const char* q = p;
		
		bool needsQuoting = false;
		
		std::string result = "'";
		
		while ( *p != '\0' )
		{
			while ( *q != '\0'  &&  *q != '\'' )
			{
				needsQuoting = needsQuoting || !std::isalnum( *q );
				++q;
			}
			
			result += std::string( p, q );
			
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
	
	
	static std::string MakeCommand( char const *const *begin, char const *const *end, bool needToEscape )
	{
		std::string command;
		
		for ( char const *const *it = begin;  it < end;  ++it )
		{
			const char* word = *it;
			
			command += needToEscape ? QuoteForMPW( word ) : word;
			
			command += " ";
		}
		
		if ( !command.empty() )
		{
			command.resize( command.size() - 1 );
		}
		
		return command;
	}
	
	int Main( int argc, iota::argv_t argv )
	{
		bool escapeForMPW = false;
		bool switchLayers = false;
		
		o::bind_option_to_variable( "--escape", escapeForMPW );
		o::bind_option_to_variable( "--switch", switchLayers );
		
		o::get_options( argc, argv );
		
		char const *const *free_args = o::free_arguments();
		
		std::string command = MakeCommand( free_args,
		                                   free_args + o::free_argument_count(),
		                                   escapeForMPW );
		
		try
		{
			return RunCommandInToolServer( command, switchLayers );
		}
		catch ( const N::OSStatus& err )
		{
			std::string status = "OSStatus ";
			
			status += iota::inscribe_decimal( err );
			
			p7::perror( "tlsrvr", status, 0 );
			
			p7::throw_errno( OSErrno::ErrnoFromOSStatus( err ) );
		}
		
		// Not reached
		return 0;
	}

}

