/*	============
 *	load-init.cc
 *	============
 */

// Mac OS X
#ifdef __APPLE__
#include <CoreServices/CoreServices.h>
#endif

// Mac OS
#ifndef __MIXEDMODE__
#include <MixedMode.h>
#endif

// Standard C
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// iota
#include "iota/strings.hh"

// command
#include "command/get_option.hh"

// gear
#include "gear/inscribe_decimal.hh"
#include "gear/parse_decimal.hh"
#include "gear/quad.hh"

// plus
#include "plus/var_string.hh"

// Nitrogen
#include "Mac/Toolbox/Types/OSStatus.hh"
#include "Mac/Toolbox/Utilities/ThrowOSStatus.hh"

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
#include "Orion/Main.hh"


using namespace command::constants;

enum
{
	Option_last_byte = 255,
	
	Option_type,
	Option_id,
};

static command::option options[] =
{
	{ "type", Option_type, Param_required },
	{ "id",   Option_id,   Param_required },
	
	{ NULL }
};

static const char* type = "INIT";
static const char* id   = "0";

static char* const* get_options( char* const* argv )
{
	++argv;  // skip arg 0
	
	short opt;
	
	while ( (opt = command::get_option( &argv, options )) )
	{
		using namespace tool;
		
		switch ( opt )
		{
			case Option_type:
				type = command::global_result.param;
				break;
			
			case Option_id:
				id = command::global_result.param;
				break;
			
			default:
				abort();
		}
	}
	
	return argv;
}


namespace tool
{
	
	namespace n = nucleus;
	namespace N = Nitrogen;
	namespace p7 = poseven;
	namespace Div = Divergence;
	
	
	static int LoadInit( const char* type, const char* id, const char *const * args )
	{
		if ( strlen( type ) != sizeof 'quad' )
		{
			Mac::ThrowOSStatus( paramErr );
		}
		
		N::ResType resType = N::ResType( gear::decode_quad( type ) );
		
		N::ResID   resID   = N::ResID( gear::parse_decimal( id ) );
		
		const char* file = args[0];
		
		n::owned< N::ResFileRefNum > resFile = N::FSpOpenResFile( Div::ResolvePathToFSSpec( file ), Mac::fsRdWrPerm );
		
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
			fprintf( stderr, "load-init: %s: returned %d\n", file, result );
		}
		
	#endif
		
		return 0;
	}
	
	int Main( int argc, char** argv )
	{
		char *const *args = get_options( argv );
		
		const int argn = argc - (args - argv);
		
		if ( argn == 0 )
		{
			p7::write( p7::stderr_fileno, STR_LEN( "Usage: load-init --type=TYPE --id=nnn file\n" ) );
			
			return 2;
		}
		
		try
		{
			return LoadInit( type, id, args );
		}
		catch ( const Mac::OSStatus& err )
		{
			plus::var_string status = "OSStatus ";
			
			status += gear::inscribe_decimal( err );
			
			p7::perror( "load-init", status, 0 );
			
			p7::throw_errno( OSErrno::ErrnoFromOSStatus( err ) );
		}
		
		// Not reached
		return 0;
	}

}
