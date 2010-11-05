/*	=======
 *	Main.cc
 *	=======
 */

#include "Orion/Main.hh"

// Standard C++
#include <exception>

// Standard C
#include <errno.h>

// plus
#include "plus/var_string.hh"

// Recall
#include "recall/backtrace.hh"
#include "recall/stack_crawl.hh"

// poseven
#include "poseven/functions/perror.hh"
#include "poseven/functions/write.hh"
#include "poseven/types/errno_t.hh"
#include "poseven/types/exit_t.hh"


namespace Orion
{
	
	namespace p7 = poseven;
	
	extern "C" int main( int argc, char** argv );
	
	
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
			const unsigned size = debugging.get_stack_crawl_size();
			
			if ( size < 2 )
			{
				return;
			}
			
			const frame_data* begin = debugging.get_stack_crawl_data();
			
			const frame_data* end = begin + size;
			
			++begin;  // skip recall::debugging_context::debugging_context( void )
			
			if ( begin->frame_pointer < stack_bottom_limit )
			{
				const frame_data* last = end - 1;
				
				while ( last->frame_pointer >= stack_bottom_limit )
				{
					--last;
				}
				
				end = last + 1;
			}
			
			plus::var_string report;
			
			make_report_from_stack_crawl( report, begin, end );
			
			p7::write( p7::stderr_fileno, report );
			
			return;
		}
		catch ( ... )
		{
		}
		
	#endif
	}
	
	
	int main( int argc, char** argv )
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
		
		return 1;  // EXIT_FAILURE
	}
	
}

