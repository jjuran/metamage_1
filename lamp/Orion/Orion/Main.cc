/*	=======
 *	Main.cc
 *	=======
 */

#include "Orion/Main.hh"

// Nitrogen
#include "Nitrogen/OSStatus.h"

// Orion
#include "Orion/StandardIO.hh"


namespace Orion
{
	
	namespace N = Nitrogen;
	
	extern "C" int main( int argc, const char* const argv[] );
	
#pragma export on
	
	int main( int argc, const char* const argv[] )
	{
		try
		{
			return Main( argc, argv );
		}
		catch ( ExitStatus& exit )
		{
			return exit.Get();
		}
		catch ( N::OSStatus& err )
		{
			Io::Err << "Orion: Main() threw OSStatus " << err.Get() << ".\n";
			return 1;
		}
		catch ( std::exception& e )
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

