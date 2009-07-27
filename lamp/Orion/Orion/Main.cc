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
#include <cstring>

// Standard C
#include <errno.h>

// Backtrace
#include "Backtrace/Backtrace.hh"
#include "Backtrace/StackCrawl.hh"

// Nucleus
#include "Nucleus/ErrorCode.h"
#include "Nucleus/Exception.h"

// POSeven
#include "POSeven/Errno.hh"
#include "POSeven/FileDescriptor.hh"
#include "POSeven/types/exit_t.hh"


namespace Orion
{
	
	namespace NN = Nucleus;
	namespace p7 = poseven;
	
	extern "C" int main( int argc, iota::argv_t argv );
	
	
	static void ShowDebuggingContext()
	{
	#if TARGET_CONFIG_DEBUGGING
		
		try
		{
			throw;
		}
		catch ( const NN::DebuggingContext& debugging )
		{
			using namespace Backtrace;
			
			const std::vector< FrameData >& stackCrawl = debugging.GetStackCrawl();
			
			if ( stackCrawl.size() < 2 )
			{
				return;
			}
			
			std::vector< FrameData >::const_iterator begin = stackCrawl.begin();
			std::vector< FrameData >::const_iterator end   = stackCrawl.end();
			
			++begin;  // skip Backtrace::DebuggingContext::DebuggingContext( void )
			
			std::string prefix = "Nucleus::Throw< Nucleus::ErrorCode< ";
			
			if ( std::equal( prefix.begin(),
			                 prefix.end(),
			                 GetCallInfoFromReturnAddress( *begin ).itsUnmangledName.begin() ) )
			{
				// Skip Nucleus::Throw< Nucleus::ErrorCode< T, i > >( void )
				// Skip Nucleus::ThrowErrorCode< T >( T )
				begin += 2;
			}
			
			std::string report = MakeReportFromStackCrawl( begin, end );
			
			p7::write( p7::stderr_fileno, report.data(), report.size() );
			
			return;
		}
		catch ( ... )
		{
		}
		
	#endif
	}
	
	
	int main( int argc, iota::argv_t argv )
	{
		const void* stackBottom = Backtrace::GetStackFramePointer();
		
		Backtrace::SetStackBottomLimit( stackBottom );
		
		try
		{
			return tool::Main( argc, argv );
		}
		catch ( const p7::exit_t& status )
		{
			return status;
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
	
}

