/*	=======
 *	Main.cc
 *	=======
 */

#include "Orion/Main.hh"

// Standard C
#include "errno.h"

// Nitrogen
#include "Nitrogen/OSStatus.h"

// POSeven
#include "POSeven/Errno.hh"

// OSErrno
#include "OSErrno/OSErrno.hh"

// Orion
#include "Orion/StandardIO.hh"


namespace Orion
{
	
	namespace N = Nitrogen;
	namespace NN = Nucleus;
	namespace P7 = POSeven;
	
	static void RegisterMacToUnixErrorConversions()
	{
		NN::RegisterExceptionConversion< P7::Errno, N::OSStatus >();
	}
	
	extern "C" int main( int argc, const char* const argv[] );
	
#pragma export on
	
	int main( int argc, const char* const argv[] )
	{
		RegisterMacToUnixErrorConversions();
		
		try
		{
			return Main( argc, argv );
		}
		catch ( const ExitStatus& exit )
		{
			return exit.Get();
		}
		catch ( const N::OSStatus& err )
		{
			Io::Err << "Orion: Main() threw OSStatus " << err.Get() << ".\n";
			
			P7::Errno errnum = NN::Convert< P7::Errno >( NN::TheExceptionBeingHandled() );
			
			if ( errnum != -1 )
			{
				Io::Err << "That converts to errno " << errnum << ": " << std::strerror( errnum ) << "\n";
			}
			
			return 1;
		}
		catch ( const P7::Errno& err )
		{
			Io::Err << "Orion: Main() threw errno " << err.Get() << ": " << std::strerror( err ) << ".\n";
			return 1;
		}
		catch ( const std::exception& e )
		{
			Io::Err << e.what() << "\n";
			return 1;
		}
		catch ( ... )
		{
			Io::Err << "Kerosene: Uncaught exception.\n";
			return 1;
		}
	}
	
#pragma export reset
	
}

