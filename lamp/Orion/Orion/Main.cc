/*	=======
 *	Main.cc
 *	=======
 */

#include "Orion/Main.hh"

// Standard C++
#include <algorithm>
#include <functional>

// Standard C/C++
#include <cstdio>

// Standard C
#include <errno.h>

// Nucleus
#include "Nucleus/ErrorCode.h"
#include "Nucleus/Exception.h"

// POSeven
#include "POSeven/Errno.hh"
#include "POSeven/FileDescriptor.hh"


#if TARGET_CPU_68K

extern "C" far void __InitCode__();

#endif


namespace Orion
{
	
	namespace NN = Nucleus;
	namespace p7 = poseven;
	
	extern "C" int main( int argc, argv_t argv );
	
	
	static void ShowDebuggingContext()
	{
		// NullDebuggingContext::Show() correctly does nothing,
		// but there's no sense in throwing needlessly
		if ( TARGET_CONFIG_DEBUGGING )
		{
			try
			{
				throw;
			}
			catch ( const NN::DebuggingContext& debugging )
			{
				using namespace Backtrace;
				
				const std::vector< ReturnAddress >& stackCrawl = debugging.GetStackCrawl();
				
				if ( stackCrawl.size() < 2 )
				{
					return;
				}
				
				std::vector< ReturnAddress >::const_iterator begin = stackCrawl.begin();
				std::vector< ReturnAddress >::const_iterator end   = stackCrawl.end();
				
				++begin;  // skip Backtrace::DebuggingContext::DebuggingContext( void )
				
				std::vector< CallInfo > callChain;
				
				callChain.reserve( end - begin );
				
				std::string prefix = "Nucleus::Throw< Nucleus::ErrorCode< ";
				
				callChain.push_back( GetCallInfoFromReturnAddress( *begin ) );
				
				if ( std::equal( prefix.begin(),
				                 prefix.end(),
				                 callChain[0].itsUnmangledName.begin() ) )
				{
					// Skip Nucleus::Throw< Nucleus::ErrorCode< T, i > >( void )
					// Skip Nucleus::ThrowErrorCode< T >( T )
					begin += 2;
					
					callChain[0] = GetCallInfoFromReturnAddress( *begin );
				}
				
				while ( callChain.back().itsUnmangledName != "main"  &&  ++begin < end )
				{
					callChain.push_back( GetCallInfoFromReturnAddress( *begin ) );
				}
				
				std::string report = MakeReportFromCallChain( callChain.begin(),
				                                              callChain.end() );
				
				p7::write( p7::stderr_fileno, report.data(), report.size() );
				
				return;
			}
			catch ( ... )
			{
			}
		}
	}
	
	
#pragma export on
	
	int main( int argc, argv_t argv )
	{
		try
		{
		#if TARGET_CPU_68K
			
			__InitCode__();
			
		#endif
			
			return Main( argc, argv );
		}
		catch ( const ExitStatus& exit )
		{
			return exit.Get();
		}
		catch ( const p7::errno_t& err )
		{
			std::fprintf( stderr, "%s: exception: %s\n", argv[0], std::strerror( err ) );
			
			ShowDebuggingContext();
		}
		catch ( const std::exception& e )
		{
			std::fprintf( stderr, "%s: exception: %s\n", argv[0], e.what() );
			
			ShowDebuggingContext();
		}
		catch ( ... )
		{
			try
			{
				NN::Exception e = NN::Convert< NN::Exception >( NN::TheExceptionBeingHandled() );
				
				std::fprintf( stderr, "%s: exception: %s\n", argv[0], e.what() );
			}
			catch ( ... )
			{
				std::fprintf( stderr, "%s: uncaught exception\n", argv[0] );
			}
			
			ShowDebuggingContext();
		}
		
		return EXIT_FAILURE;
	}
	
#pragma export reset
	
}

