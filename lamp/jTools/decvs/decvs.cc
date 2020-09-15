/*
	decvs.cc
	--------
*/

// Standard C
#include <stdio.h>
#include <stdlib.h>

// command
#include "command/get_option.hh"

// Extended API Set, Part 2
#include "extended-api-set/part-2.h"

// iota
#include "iota/strings.hh"

// plus
#include "plus/var_string.hh"

// poseven
#include "poseven/functions/dirfd.hh"
#include "poseven/functions/opendir.hh"
#include "poseven/types/fd_t.hh"

// Nitrogen
#include "Nitrogen/Files.hh"
#include "Nitrogen/Resources.hh"

// OSErrno
#include "OSErrno/OSErrno.hh"

// Divergence
#include "Divergence/Utilities.hh"

// Orion
#include "Orion/Main.hh"


#define ARRAYLEN( array )  (sizeof array / sizeof array[0])

#define PSTR_LEN( s )  "\p" STR_LEN( s )


using namespace command::constants;

enum
{
	Option_quiet     = 'q',
	Option_recursive = 'r',
};

static command::option options[] =
{
	{ "", Option_quiet     },
	{ "", Option_recursive },
	
	{ NULL }
};

static bool globally_recursive = false;
static bool quiet = false;

static char* const* get_options( char* const* argv )
{
	++argv;  // skip arg 0
	
	short opt;
	
	while ( (opt = command::get_option( &argv, options )) )
	{
		switch ( opt )
		{
			case Option_quiet:
				quiet = true;
				break;
			
			case Option_recursive:
				globally_recursive = true;
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
	
	
	static const char* const cvs_filenames[] =
	{
		".",
		"..",
		"Entries",
		"Repository",
		"Root",
		"Tag"
	};
	
	static bool contains_string( const char* const* begin, const char* const* end, const char* s )
	{
		while ( begin < end )
		{
			if ( strcmp( *begin++, s ) == 0 )
			{
				return true;
			}
		}
		
		return false;
	}
	
	static bool is_CVS_filename( const char* name )
	{
		return contains_string( cvs_filenames,
		                        cvs_filenames + ARRAYLEN( cvs_filenames ),
		                        name );
	}
	
	static void remove_CVS_files( p7::fd_t dirfd )
	{
		const char* const* begin = cvs_filenames + 2;
		const char* const* end   = begin + ARRAYLEN( cvs_filenames );
		
		while ( begin < end )
		{
			(void) unlinkat( dirfd, *begin++, 0 );
		}
	}
	
	static bool decvs_file( const char* path );
	
	static void decvs_dir( const char* path )
	{
		n::owned< p7::dir_t > dir = p7::opendir( path );
		
		plus::var_string subpath = path;
		
		subpath += '/';
		
		const size_t path_len = subpath.size();
		
		while ( const dirent* entry = ::readdir( dir ) )
		{
			const char* name = entry->d_name;
			
			if ( name[0] == '.'  &&  name[ 1 + (name[1] == '.') ] == '\0' )
			{
				continue;
			}
			
			subpath.resize( path_len );
			
			subpath += name;
			
			if ( decvs_file( subpath.c_str() ) )
			{
				printf( "%s\n", subpath.c_str() );
			}
		}
	}
	
	static bool decvs_file( const char* path )
	{
		FSSpec file = Divergence::ResolvePathToFSSpec( path );
		
		CInfoPBRec cInfo = { 0 };
		
		N::Str255 name = file.name;
		
		nucleus::initialize< CInfoPBRec >( cInfo,
		                                   Mac::FSVolumeRefNum( file.vRefNum ),
		                                   Mac::FSDirID       ( file.parID   ),
		                                   name,
		                                   0 );
		
		N::PBGetCatInfoSync( cInfo, N::FNF_Throws() );
		
		HFileInfo& hFileInfo = cInfo.hFileInfo;
		
		if ( hFileInfo.ioFlAttrib & kioFlAttribDirMask )
		{
			if ( memcmp( name, PSTR_LEN( "CVS" ) ) == 0 )
			{
				bool has_other_files = false;
				
				n::owned< p7::dir_t > dir = p7::opendir( path );
				
				while ( const dirent* entry = ::readdir( dir ) )
				{
					if ( !is_CVS_filename( entry->d_name ) )
					{
						has_other_files = true;
						
						break;
					}
				}
				
				if ( !has_other_files )
				{
					p7::fd_t dirfd = p7::dirfd( dir );
					
					remove_CVS_files( dirfd );
					
					(void) rmdir( path );
					
					return true;
				}
			}
			
			if ( globally_recursive )
			{
				decvs_dir( path );
			}
			
			return false;
		}
		
		if ( hFileInfo.ioFlRLgLen == 0 )
		{
			// Empty resource fork
			return false;
		}
		
		const UInt32 saved_mod_time = hFileInfo.ioFlMdDat;
		
		const size_t empty_rsrc_map_size = 286;
		
		bool empty_map = hFileInfo.ioFlRLgLen == empty_rsrc_map_size;
		
		bool modified = empty_map;
		
		if ( !empty_map )
		{
			n::owned< N::ResFileRefNum > resFile = N::FSpOpenResFile( file, N::fsRdWrPerm );
			
			Handle ckid = ::Get1Resource( 'ckid', 128 );
			
			if ( ckid )
			{
				(void) N::RemoveResource( ckid );
				
				modified = true;
			}
			
			if ( N::Count1Types() == 0 )
			{
				empty_map = true;
			}
		}
		
		if ( empty_map )
		{
			n::owned< N::FSFileRefNum > rsrcFork = N::FSpOpenRF( file, N::fsRdWrPerm );
			
			N::SetEOF( rsrcFork, 0 );
		}
		
		if ( modified )
		{
			hFileInfo.ioFlMdDat = saved_mod_time;
			
			hFileInfo.ioDirID = file.parID;
			
			N::PBSetCatInfoSync( cInfo );
		}
		
		return modified;
	}
	
	int Main( int argc, char** argv )
	{
		char *const *args = get_options( argv );
		
		const int argn = argc - (args - argv);
		
		int exit_status = 0;
		
		for ( ;  *args != NULL;  ++args )
		{
			const char* path = *args;
			
			try
			{
				const bool modified = decvs_file( path );
				
				if ( !quiet  &&  modified )
				{
					printf( "%s\n", path );
				}
			}
			catch ( const Mac::OSStatus& err )
			{
				try
				{
					errno = OSErrno::ErrnoFromOSStatus( err );
					
					perror( path );
					
					exit_status = 1;
				}
				catch ( ... )
				{
					fprintf( stderr, "%s: OSStatus %ld\n", path, err.Get() );
				}
			}
		}
		
		return exit_status;
	}
	
}
