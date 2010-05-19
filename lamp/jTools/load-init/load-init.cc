/*	============
 *	load-init.cc
 *	============
 */

// Mac OS
#ifndef __MIXEDMODE__
#include <MixedMode.h>
#endif

// Standard C++
#include <functional>
#include <vector>

// iota
#include "iota/decimal.hh"
#include "iota/quad.hh"
#include "iota/strings.hh"

// plus
#include "plus/var_string.hh"

// Nitrogen
#include "Nitrogen/OSStatus.hh"
#include "Nitrogen/Resources.hh"
#include "Nitrogen/Str.hh"

// poseven
#include "poseven/functions/perror.hh"
#include "poseven/functions/write.hh"

// Divergence
#include "Divergence/Utilities.hh"

// OSErrno
#include "OSErrno/OSErrno.hh"

// Orion
#include "Orion/get_options.hh"
#include "Orion/Main.hh"


namespace tool
{
	
	namespace n = nucleus;
	namespace N = Nitrogen;
	namespace p7 = poseven;
	namespace Div = Divergence;
	namespace o = orion;
	
	
	static int LoadInit( const char* type, const char* id, iota::argv_t args )
	{
		if ( strlen( type ) != sizeof 'quad' )
		{
			N::ThrowOSStatus( paramErr );
		}
		
		N::ResType resType = N::ResType( iota::decode_quad( type ) );
		
		N::ResID   resID   = N::ResID( iota::parse_decimal( id ) );
		
		const char* file = args[0];
		
		n::owned< N::ResFileRefNum > resFile = N::FSpOpenResFile( Div::ResolvePathToFSSpec( file ), N::fsRdWrPerm );
		
		N::Handle handle = N::Get1Resource( resType, resID );
		
		N::HLock( handle );
		
	#if TARGET_API_MAC_CARBON
		
		p7::write( p7::stderr_fileno, STR_LEN( "load-init: Can't run INITs in Carbon\n" ) );
		
		return 1;
		
	#else
		
		typedef int (*Code)();
		
		Code code = reinterpret_cast< Code >( *handle.Get() );
		
		ProcInfoType procInfo = kCStackBased
		                      | RESULT_SIZE( SIZE_CODE( sizeof (int) ) );
		
	#if TARGET_RT_MAC_CFM
		
		typedef UniversalProcPtr CodeUPP;
		
	#else
		
		typedef Code CodeUPP;
		
	#endif
		
		const CodeUPP upp = (CodeUPP) code;
				
		int result = CALL_ZERO_PARAMETER_UPP( upp, procInfo );
		
		if ( result != 0 )
		{
			std::fprintf( stderr, "load-init: %s: returned %d\n", file, result );
		}
		
	#endif
		
		return EXIT_SUCCESS;
	}
	
	int Main( int argc, iota::argv_t argv )
	{
		const char* type = "INIT";
		const char* id   = "0";
		
		o::bind_option_to_variable( "--type", type );
		o::bind_option_to_variable( "--id",   id   );
		
		o::get_options( argc, argv );
		
		iota::argp_t freeArgs = o::free_arguments();
		
		const size_t n_args = o::free_argument_count();
		
		if ( n_args == 0 )
		{
			p7::write( p7::stderr_fileno, STR_LEN( "Usage: load-init --type=TYPE --id=nnn file\n" ) );
			
			return 2;
		}
		
		try
		{
			return LoadInit( type, id, freeArgs );
		}
		catch ( const N::OSStatus& err )
		{
			plus::var_string status = "OSStatus ";
			
			status += iota::inscribe_decimal( err );
			
			p7::perror( "load-init", status, 0 );
			
			p7::throw_errno( OSErrno::ErrnoFromOSStatus( err ) );
		}
		
		// Not reached
		return 0;
	}

}

