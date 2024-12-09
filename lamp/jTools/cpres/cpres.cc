/*	========
 *	cpres.cc
 *	========
 */

// Standard C
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// mac-sys-utils
#include "mac_sys/res_error.hh"

// command
#include "command/get_option.hh"

// Nitrogen
#include "Mac/Toolbox/Types/OSStatus.hh"
#include "Mac/Toolbox/Utilities/ThrowOSStatus.hh"

// poseven
#include "poseven/functions/stat.hh"
#include "poseven/types/mode_t.hh"

// Orion
#include "Orion/Main.hh"

// cpres
#include "res_file.hh"


namespace n = nucleus;
namespace N = Nitrogen;
namespace p7 = poseven;


enum
{
	Option_last_byte = 255,
	
	Option_data_fork,
};

static command::option options[] =
{
	{ "data",      Option_data_fork },
	{ "data-fork", Option_data_fork },
	{ NULL }
};

static bool globally_using_data_fork = false;


static char* const* get_options( char* const* argv )
{
	++argv;  // skip arg 0
	
	short opt;
	
	while ( (opt = command::get_option( &argv, options )) )
	{
		switch ( opt )
		{
			case Option_data_fork:
				globally_using_data_fork = true;
				break;
			
			default:
				abort();
		}
	}
	
	return argv;
}


namespace tool
{
	
	static bool has_BNDL = false;
	
	static int TryResCopy( const char*       source_path,
	                       N::ResFileRefNum  destRes,
	                       ForkType          fork )
	{
		struct stat st = p7::stat( source_path );
		
		if ( p7::s_isdir( st ) )
		{
			// Source item is a directory.
			fprintf( stderr, "cpres: %s: omitting directory\n", source_path );
			
			return 1;
		}
		
		n::owned< N::ResFileRefNum > sourceRes = open_res_file( source_path, fork );
		
		int types = N::Count1Types();
		
		for ( int iType = 1;  iType <= types;  ++iType )
		{
			N::ResType type = N::Get1IndType( iType );
			
			if ( type == 'BNDL' )
			{
				has_BNDL = true;
			}
			
			int rsrcs = N::Count1Resources( type );
			
			for ( int iRsrc = 1;  iRsrc <= rsrcs;  ++iRsrc )
			{
				using mac::sys::res_error;
				
				OSErr err;
				
				Handle h = Get1IndResource( type, iRsrc );
				
				if ( ! h )
				{
					if ( ! (err = res_error()) )
					{
						err = resNotFound;
					}
					
					goto error;
				}
				
				HNoPurge( h );
				
				UseResFile( destRes );
				
				ResType type;
				short   id;
				Str255  name;
				
				GetResInfo( h, &id, &type, name );
				
				if ( Handle existing = Get1Resource( type, id ) )
				{
					RemoveResource( existing );
				}
				else if ( (err = res_error()) != resNotFound )
				{
					goto error;
				}
				else
				{
					// Okay, resource didn't exist in dest file
				}
				
				DetachResource( h );
				
				AddResource( h, type, id, name );
				
				err = res_error();
				
			error:
				
				UseResFile( sourceRes );
				
				Mac::ThrowOSStatus( err );
			}
		}
		
		return 0;
	}
	
	
	int Main( int argc, char** argv )
	{
		char *const *args = get_options( argv );
		
		const int argn = argc - (args - argv);
		
		if ( globally_using_data_fork  &&  ! has_FSOpenResourceFile() )
		{
			fprintf( stderr, "cpres: FSOpenResourceFile() unavailable for data fork\n" );
			
			return 2;
		}
		
		int fail = 0;
		
		// Check for sufficient number of args
		if ( argn < 2 )
		{
			fprintf( stderr, "cpres: missing %s\n", (argn == 0) ? "file arguments"
			                                                    : "destination file" );
			
			return 1;
		}
		
		// Last arg should be the destination file.
		const char* dest_path = args[ argn - 1 ];
		
		struct stat st;
		
		const bool exists = p7::stat( dest_path, st );
		
		if ( exists  &&  p7::s_isdir( st ) )
		{
			fprintf( stderr, "cpres: last argument (%s) is not a file.\n", dest_path );
			
			return 1;
		}
		
		ForkType fork = globally_using_data_fork ? dataFork : rsrcFork;
		
		n::owned< N::ResFileRefNum > resFileH = open_res_file( dest_path, fork, exists );
		
		// Try to copy each file.  Return whether any errors occurred.
		for ( int index = 0;  index < argn - 1;  ++index )
		{
			const char* source_path = args[ index ];
			
			try
			{
				fail += TryResCopy( source_path, resFileH, fork );
			}
			catch ( const Mac::OSStatus& err )
			{
				++fail;
				
				const char* slash = strrchr( dest_path, '/' );
				const char* dest_name = slash ? slash + 1 : dest_path;
				
				fprintf( stderr, "OSStatus %ld copying from %s to %s.\n",
				                           err.Get(),       source_path,
				                                                  dest_name );
			}
		}
		
		set_BNDL_bit( dest_path, has_BNDL );
		
		return fail;
	}
	
}
