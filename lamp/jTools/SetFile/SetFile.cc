/*	==========
 *	SetFile.cc
 *	==========
 */

// Standard C
#include <stdlib.h>

// command
#include "command/get_option.hh"

// gear
#include "gear/inscribe_decimal.hh"
#include "gear/quad.hh"

// plus
#include "plus/var_string.hh"

// poseven
#include "poseven/functions/perror.hh"

// Nitrogen
#include "Mac/Toolbox/Types/OSStatus.hh"

#include "Nitrogen/Files.hh"

// Divergence
#include "Divergence/Utilities.hh"

// OSErrno
#include "OSErrno/OSErrno.hh"

// Orion
#include "Orion/Main.hh"


using namespace command::constants;

enum
{
	Option_creator = 'c',
	Option_type    = 't',
};

static command::option options[] =
{
	{ "", Option_creator, Param_required },
	{ "", Option_type,    Param_required },
	
	{ NULL }
};

static const char* creator_arg = NULL;
static const char* type_arg    = NULL;

static OSType the_creator;
static OSType the_type;

static char* const* get_options( char* const* argv )
{
	++argv;  // skip arg 0
	
	short opt;
	
	while ( (opt = command::get_option( &argv, options )) )
	{
		switch ( opt )
		{
			case Option_creator:
				creator_arg = command::global_result.param;
				the_creator = gear::decode_quad( creator_arg );
				break;
			
			case Option_type:
				type_arg = command::global_result.param;
				the_type = gear::decode_quad( type_arg );
				break;
			
			default:
				abort();
		}
	}
	
	return argv;
}


namespace tool
{

namespace N = Nitrogen;
namespace p7 = poseven;
namespace Div = Divergence;


static void SetFileInfo( const FSSpec& file )
{
	CInfoPBRec cInfo;
	
	N::FSpGetCatInfo( file, cInfo );
	
	FInfo& fInfo = cInfo.hFileInfo.ioFlFndrInfo;
	
	if ( creator_arg )
	{
		fInfo.fdCreator = the_creator;
	}
	
	if ( type_arg )
	{
		fInfo.fdType = the_type;
	}
	
	cInfo.hFileInfo.ioNamePtr = const_cast< StringPtr >( file.name );
	cInfo.hFileInfo.ioDirID   = file.parID;
	
	N::PBSetCatInfoSync( cInfo );
}


int Main( int argc, char** argv )
{
	char *const *args = get_options( argv );
	
	const int argn = argc - (args - argv);
	
	char const *const *free_args = args;
	
	int exit_status = 0;
	
	while ( *free_args )
	{
		const char* pathname = *free_args++;
		
		try
		{
			SetFileInfo( Div::ResolvePathToFSSpec( pathname ) );
		}
		catch ( const Mac::OSStatus& err )
		{
			plus::var_string status = "OSStatus ";
			
			status += gear::inscribe_decimal( err );
			
			const int errnum = OSErrno::ErrnoFromOSStatus( err );
			
			p7::perror( "SetFile", pathname, status );
			p7::perror( "SetFile", pathname, errnum );
			
			exit_status = 1;
		}
	}
	
	return exit_status;
}

}
