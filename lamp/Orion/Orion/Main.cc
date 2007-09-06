/*	=======
 *	Main.cc
 *	=======
 */

#include "Orion/Main.hh"

// Standard C
#include "errno.h"

// Nucleus
#include "Nucleus/ErrorCode.h"
#include "Nucleus/Exception.h"

// Nitrogen
//#include "Nitrogen/OSStatus.h"

// POSeven
#include "POSeven/Errno.hh"

// OSErrno
//#include "OSErrno/OSErrno.hh"

// Orion
#include "Orion/StandardIO.hh"


#if TARGET_CPU_68K

extern "C" far void __InitCode__();

#endif


namespace Orion
{
	
	//namespace N = Nitrogen;
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
		/*
		catch ( const N::OSStatus& err )
		{
			Io::Err << "Orion: Main() threw OSStatus " << err.Get() << ".\n";
			
			NN::RegisterExceptionConversion< P7::Errno, N::OSStatus >();
			
			P7::Errno errnum = NN::Convert< P7::Errno >( NN::TheExceptionBeingHandled() );
			
			if ( errnum != -1 )
			{
				Io::Err << "That converts to errno " << errnum << ": " << std::strerror( errnum ) << "\n";
			}
			
			return 1;
		}
		*/
		catch ( const P7::Errno& err )
		{
			ShowDebuggingContext();
			
			Io::Err << "Orion: Main() threw errno " << err.Get() << ": " << std::strerror( err ) << ".\n";
			return 1;
		}
		catch ( const std::exception& e )
		{
			Io::Err << "Exception: " << e.what() << "\n";
			return 1;
		}
		catch ( ... )
		{
			//NN::RegisterExceptionConversion< NN::Exception, N::OSStatus >();
			
			ShowDebuggingContext();
			
			try
			{
				NN::Exception e = NN::Convert< NN::Exception >( NN::TheExceptionBeingHandled() );
				
				Io::Err << "Exception: " << e.what() << "\n";
			}
			catch ( ... )
			{
				Io::Err << "Kerosene: Uncaught exception.\n";
			}
			
			
			return 1;
		}
	}
	
#pragma export reset
	
}

