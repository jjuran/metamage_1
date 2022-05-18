/*	========
 *	cpres.cc
 *	========
 */

// Standard C
#include <stdlib.h>

// Standard C/C++
#include <cstdio>

// mac-sys-utils
#include "mac_sys/res_error.hh"

// command
#include "command/get_option.hh"

// nucleus
#include "nucleus/saved.hh"

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
			std::fprintf( stderr, "cpres: %s: omitting directory\n", source_path );
			
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
				N::Handle h = N::Get1IndResource( type, iRsrc );
				
				::HNoPurge( h );
				
				n::saved< N::ResFile > savedResFile( destRes );
				
				mac::types::ResInfo resInfo = N::GetResInfo( h );
				
				Handle existing = ::Get1Resource( resInfo.type, resInfo.id );
				
				if ( existing )
				{
					::RemoveResource( existing );
				}
				else
				{
					OSErr err = mac::sys::res_error();
					
					if ( err != resNotFound )
					{
						Mac::ThrowOSStatus( err );
					}
					
					// Okay, resource didn't exist in dest file
				}
				
				N::AddResource( N::DetachResource( h ), resInfo );
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
			std::fprintf( stderr, "cpres: FSOpenResourceFile() unavailable for data fork\n" );
			
			return 2;
		}
		
		int fail = 0;
		
		// Check for sufficient number of args
		if ( argn < 2 )
		{
			std::fprintf( stderr, "cpres: missing %s\n", (argn == 0) ? "file arguments"
			                                                         : "destination file" );
			
			return 1;
		}
		
		// Last arg should be the destination file.
		const char* dest_path = args[ argn - 1 ];
		
		struct stat st;
		
		const bool exists = p7::stat( dest_path, st );
		
		if ( exists  &&  p7::s_isdir( st ) )
		{
			std::fprintf( stderr, "cpres: last argument (%s) is not a file.\n", dest_path );
			
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
				
				std::fprintf( stderr, "OSStatus %ld copying from %s to %s.\n",
				                                err.Get(),       source_path,
				                                                       dest_name );
			}
		}
		
		set_BNDL_bit( dest_path, has_BNDL );
		
		return fail;
	}
	
}
