/*	============
 *	load-init.cc
 *	============
 */

// Standard C++
#include <functional>
#include <vector>

// Iota
#include "iota/strings.hh"

// Nitrogen
#include "Nitrogen/MacErrors.h"
#include "Nitrogen/OSStatus.h"
#include "Nitrogen/Resources.h"
#include "Nitrogen/Str.h"

// POSeven
#include "POSeven/FileDescriptor.hh"

// Divergence
#include "Divergence/Utilities.hh"

// Orion
#include "Orion/GetOptions.hh"
#include "Orion/Main.hh"


namespace Nucleus
{
	
	template <> struct Converter< Nitrogen::ResType, std::string > : public Nitrogen::StringToFourCharCode_Converter< Nitrogen::ResType > {};
	
	template <> struct Converter< std::string, Nitrogen::ResType > : public Nitrogen::FourCharCodeToString_Converter< Nitrogen::ResType > {};
	
}

namespace N = Nitrogen;
namespace NN = Nucleus;
namespace p7 = poseven;
namespace Div = Divergence;
namespace O = Orion;


static void LoadInit( const char* type, const char* id, iota::argv_t args )
{
	N::ResType resType = NN::Convert< N::ResType >( std::string( type ) );
	
	N::ResID   resID   = N::ResID( std::atoi( id ) );
	
	const char* file = args[0];
	
	NN::Owned< N::ResFileRefNum > resFile = N::FSpOpenResFile( Div::ResolvePathToFSSpec( file ), N::fsRdWrPerm );
	
	N::Handle handle = N::Get1Resource( resType, resID );
	
	//N::HNoPurge( handle );  // not taking chances
	N::HLock   ( handle );
	
	typedef int (*Code)();
	
	Code code = reinterpret_cast< Code >( *handle.Get() );
	
	if ( TARGET_CPU_68K )
	{
		int result = code();
		
		if ( result != 0 )
		{
			std::fprintf( stderr, "load-init: %s: returned %d\n", file, result );
		}
	}
}

int O::Main( int argc, argv_t argv )
{
	NN::RegisterExceptionConversion< NN::Exception, N::OSStatus >();
	
	const char* type = "INIT";
	const char* id   = "0";
	
	O::BindOption( "--type", type );
	O::BindOption( "--id",   id   );
	
	O::GetOptions( argc, argv );
	
	char const *const *freeArgs = O::FreeArguments();
	
	std::size_t argCount = O::FreeArgumentCount();
	
	if ( argCount == 0 )
	{
		p7::write( p7::stderr_fileno, STR_LEN( "Usage: load-init --type=TYPE --id=nnn file\n" ) );
		
		return 2;
	}
	
	LoadInit( type, id, freeArgs );
	
	return EXIT_SUCCESS;
}

