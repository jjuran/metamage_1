/*	=======
 *	Main.cc
 *	=======
 */

#include "Orion/Main.hh"

// Standard C
#include <errno.h>

// Standard C/C++
#include <cstdio>

// Nucleus
#include "Nucleus/ErrorCode.h"
#include "Nucleus/Exception.h"

// POSeven
#include "POSeven/Errno.hh"


#if TARGET_CPU_68K

extern "C" far void __InitCode__();

#endif


namespace Orion
{
	
	namespace NN = Nucleus;
	namespace P7 = POSeven;
	
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
				debugging.Show();
				
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
		catch ( const P7::Errno& err )
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

