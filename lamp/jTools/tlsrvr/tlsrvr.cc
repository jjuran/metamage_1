/*	=========
 *	tlsrvr.cc
 *	=========
 */

// Standard C++
#include <algorithm>
#include <functional>
#include <memory>
#include <numeric>
#include <string>
#include <vector>

// Standard C/C++
#include <cctype>

// Nitrogen
#include "Nitrogen/Events.h"

// MoreFunctional
#include "FunctionalExtensions.hh"

// Nitrogen Extras / Utilities
#include "Utilities/Processes.h"

// Orion
#include "Orion/get_options.hh"
#include "Orion/Main.hh"

// tlsrvr
#include "ToolServer.hh"
#include "RunToolServer.hh"


namespace tool
{
	
	namespace N = Nitrogen;
	namespace NN = Nucleus;
	namespace NX = NitrogenExtras;
	namespace o = orion;
	
	
	template < class F >
	class Concat
	{
		private:
			F f;
		
		public:
			Concat() : f( F() )  {}
			Concat( const F& f ) : f( f )  {}
			std::string operator()( const std::string& one, const char* other ) const
			{
				return one + " " + f( other );
			}
	};
	
	template < class F >
	Concat< F > MakeConcat( const F& f )
	{
		return Concat< F >( f );
	}
	
	
	static std::string QuoteForMPW( const std::string& str )
	{
		std::string::const_iterator p = str.begin(), q = p, end = str.end();
		
		bool needsQuoting = false;
		
		std::string result = "'";
		
		while ( p < end )
		{
			while ( q < end  &&  *q != '\'' )
			{
				needsQuoting = needsQuoting || !std::isalnum( *q );
				++q;
			}
			
			result += std::string( p, q );
			
			if ( q < end )
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
		
		if ( end - begin > 0 )
		{
			if ( needToEscape )
			{
				command = std::accumulate
				(
					begin + 1, 
					end, 
					QuoteForMPW( begin[ 0 ] ), 
					MakeConcat( std::ptr_fun( QuoteForMPW ) )
				);
			}
			else
			{
				command = std::accumulate
				(
					begin + 1, 
					end, 
					std::string( begin[ 0 ] ), 
					Concat< more::identity< const char* > >()
				);
			}
		}
		
		return command;
	}
	
	int Main( int argc, iota::argv_t argv )
	{
		NN::RegisterExceptionConversion< NN::Exception, N::OSStatus >();
		
		bool escapeForMPW = false;
		bool switchLayers = false;
		
		o::bind_option_to_variable( "--escape", escapeForMPW );
		o::bind_option_to_variable( "--switch", switchLayers );
		
		o::get_options( argc, argv );
		
		char const *const *free_args = o::free_arguments();
		
		std::string command = MakeCommand( free_args,
		                                   free_args + o::free_argument_count(),
		                                   escapeForMPW );
		
		// This is a bit of a hack.  It really ought to happen just after we send the event,
		// but switchLayers is local to this file and I'm not dealing with that now.
		if ( switchLayers && N::SameProcess( N::CurrentProcess(),
		                                     N::GetFrontProcess() ) )
		{
			N::SetFrontProcess( NX::LaunchApplication( sigToolServer ) );
		}
		
		int result = RunCommandInToolServer( command );
		
		if ( switchLayers && N::SameProcess( NX::LaunchApplication( sigToolServer ),
		                                     N::GetFrontProcess() ) )
		{
			N::SetFrontProcess( N::CurrentProcess() );
		}
		
		return result;
	}

}

