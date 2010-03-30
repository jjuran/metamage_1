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

// poseven
#include "poseven/functions/perror.hh"
#include "poseven/functions/write.hh"
#include "poseven/types/exit_t.hh"


namespace Orion
{
	
	namespace p7 = poseven;
	
	extern "C" int main( int argc, iota::argv_t argv );
	
	
	static void ShowDebuggingContext( const void* stack_bottom_limit )
	{
	#if TARGET_CONFIG_DEBUGGING
		
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
			
			if ( begin->frame_pointer < stack_bottom_limit )
			{
				std::vector< frame_data >::const_iterator last = end - 1;
				
				while ( last->frame_pointer >= stack_bottom_limit )
				{
					--last;
				}
				
				end = last + 1;
			}
			
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
			
			ShowDebuggingContext( stackBottom );
		}
		catch ( const std::exception& e )
		{
			p7::perror( argv[0], "exception", e.what() );
			
			ShowDebuggingContext( stackBottom );
		}
		catch ( ... )
		{
			p7::perror( argv[0], "uncaught exception" );
			
			ShowDebuggingContext( stackBottom );
		}
		
		return EXIT_FAILURE;
	}
	
}

