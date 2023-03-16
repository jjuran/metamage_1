/*	==========
 *	SetFile.cc
 *	==========
 */

// Mac OS X
#ifdef __APPLE__
#include <CoreServices/CoreServices.h>
#endif

// Mac OS
#ifndef __FILES__
#include <Files.h>
#endif

// Standard C
#include <stdlib.h>

// command
#include "command/get_option.hh"

// gear
#include "gear/inscribe_decimal.hh"
#include "gear/quad.hh"

// mac-relix-utils
#include "mac_relix/FSSpec_from_path.hh"

// plus
#include "plus/var_string.hh"

// poseven
#include "poseven/functions/perror.hh"

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


static
OSErr set_FInfo( short vRefNum, long parID, ConstStr255Param name )
{
	OSErr err;
	FInfo fInfo;
	
	err = HGetFInfo( vRefNum, parID, name, &fInfo );
	
	if ( err == noErr )
	{
		if ( creator_arg )
		{
			fInfo.fdCreator = the_creator;
		}
		
		if ( type_arg )
		{
			fInfo.fdType = the_type;
		}
		
		err = HSetFInfo( vRefNum, parID, name, &fInfo );
	}
	
	return err;
}

namespace tool
{

namespace p7 = poseven;


int Main( int argc, char** argv )
{
	char *const *args = get_options( argv );
	
	const int argn = argc - (args - argv);
	
	char const *const *free_args = args;
	
	int exit_status = 0;
	
	while ( *free_args )
	{
		const char* pathname = *free_args++;
		
		OSErr err;
		FSSpec file;
		
		err = mac::relix::FSSpec_from_existing_path( pathname, file );
		
		if ( err == noErr )
		{
			err = set_FInfo( file.vRefNum, file.parID, file.name );
		}
		
		if ( err != noErr )
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
