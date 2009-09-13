/*	=======
 *	Main.cc
 *	=======
 */

#include "Orion/Main.hh"

// Standard C++
#include <algorithm>
#include <functional>

// Standard C/C++
#include <cstring>

// Standard C
#include <errno.h>

// Recall
#include "recall/backtrace.hh"
#include "recall/stack_crawl.hh"

// Nucleus
#include "Nucleus/ErrorCode.h"
#include "Nucleus/Exception.h"
#include "Nucleus/TheExceptionBeingHandled.h"

// POSeven
#include "POSeven/functions/perror.hh"
#include "POSeven/functions/write.hh"
#include "POSeven/types/exit_t.hh"


namespace Orion
{
	
	namespace NN = Nucleus;
	namespace p7 = poseven;
	
	extern "C" int main( int argc, iota::argv_t argv );
	
	
	static void ShowDebuggingContext()
	{
	#if TARGET_CONFIG_DEBUGGING && defined( NUCLEUS_USES_BACKTRACE )
		
		using namespace recall;
		
		try
		{
			throw;
		}
		catch ( const debugging_context& debugging )
		{
			const std::vector< frame_data >& stackCrawl = debugging.get_stack_crawl();
			
			if ( stackCrawl.size() < 2 )
			{
				return;
			}
			
			std::vector< frame_data >::const_iterator begin = stackCrawl.begin();
			std::vector< frame_data >::const_iterator end   = stackCrawl.end();
			
			++begin;  // skip recall::debugging_context::debugging_context( void )
			
			std::string report = make_report_from_stack_crawl( begin, end );
			
			p7::write( p7::stderr_fileno, report );
			
			return;
		}
		catch ( ... )
		{
		}
		
	#endif
	}
	
	
	int main( int argc, iota::argv_t argv )
	{
		const void* stackBottom = recall::get_stack_frame_pointer();
		
		recall::set_stack_bottom_limit( stackBottom );
		
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
			p7::perror( argv[0], "exception" );
			
			ShowDebuggingContext();
		}
		catch ( const std::exception& e )
		{
			p7::perror( argv[0], "exception", e.what() );
			
			ShowDebuggingContext();
		}
		catch ( ... )
		{
			try
			{
				NN::Exception e = NN::Convert< NN::Exception >( NN::TheExceptionBeingHandled() );
				
				p7::perror( argv[0], "exception", e.what() );
			}
			catch ( ... )
			{
				p7::perror( argv[0], "uncaught exception" );
			}
			
			ShowDebuggingContext();
		}
		
		return EXIT_FAILURE;
	}
	
}

