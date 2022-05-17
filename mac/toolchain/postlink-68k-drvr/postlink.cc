/*
	postlink.cc
	-----------
*/

// Mac OS X
#ifdef __APPLE__
#include <CoreServices/CoreServices.h>
#endif

// Mac OS
#ifndef __RESOURCES__
#include <Resources.h>
#endif
#ifndef __TEXTUTILS__
#include <TextUtils.h>
#endif

// POSIX
#include <unistd.h>

// Standard C
#include <stdlib.h>

// mac-sys-utils
#include "mac_sys/res_error.hh"

// mac-relix-utils
#include "mac_relix/FSSpec_from_path.hh"

// command
#include "command/get_option.hh"


#define PREFIX  "postlink-68k-drvr: "

#define STR_LEN( s )  "" s, (sizeof s - 1)

#define ERROR( e, msg )  (wrote( 2, STR_LEN( PREFIX msg "\n" ) ) ? e : 13)


using namespace command::constants;

enum
{
	Option_dry_run = 'n',
	Option_verbose = 'v',
};

static command::option options[] =
{
	{ "", Option_dry_run },
	{ "", Option_verbose },
	
	{ NULL }
};

static bool dry_run = false;
static bool verbose = false;

static inline
bool wrote( int fd, const void* buffer, size_t n )
{
	return write( fd, buffer, n ) == n;
}

static
char* const* get_options( char* const* argv )
{
	++argv;  // skip arg 0
	
	short opt;
	
	while ( (opt = command::get_option( &argv, options )) )
	{
		switch ( opt )
		{
			case Option_dry_run:
				dry_run = true;
				break;
			
			case Option_verbose:
				verbose = true;
				break;
			
			default:
				abort();
		}
	}
	
	return argv;
}

static
bool fix_up_DRVR_resource( Handle code )
{
	/*
		Return true if we changed anything, and false otherwise.
	*/
	
	const Size expected_minimum_size = 18 + 20;
	
	Size size = GetHandleSize( code );
	
	if ( size > expected_minimum_size  &&  *(short*) code[0] == 0x600A )
	{
		Munger( code, 0, NULL, 18, NULL, 0 );  // delete the first 18 bytes
		
		StringPtr name = (StringPtr) *code + 18;
		
		if ( name[ 1 ] == '.' )
		{
			/*
				This is a device driver (as indicated by the name beginning
				with a period ('.'), but the Metrowerks linker will name
				the resource with a preceding NUL instead.  Override it.
			*/
			
			SetResInfo( code, 0, name );
		}
		
		return true;
	}
	
	return false;
}

int main( int argc, char** argv )
{
	using mac::relix::FSSpec_from_existing_path;
	using mac::sys::res_error;
	
	char *const *args = get_options( argv );
	
	const int argn = argc - (args - argv);
	
	if ( argn == 0 )
	{
		return ERROR( 50, "argument required" );
	}
	
	const char* target_path = args[ 0 ];
	
	FSSpec target_filespec;
	
	if ( FSSpec_from_existing_path( target_path, target_filespec ) )
	{
		return 43;  // producing an FSSpec failed; presumably nonexistent
	}
	
	int exit_status = 1;  // opening the resource file failed
	
	short resfile = FSpOpenResFile( &target_filespec, fsRdWrPerm );
	
	if ( resfile >= 0 )
	{
		if ( Handle code = Get1Resource( 'DRVR', 0 ) )
		{
			exit_status = 50;  // non-conforming input (shouldn't happen)
			
			if ( fix_up_DRVR_resource( code ) )
			{
				exit_status = 0;  // dry run successful
				
				if ( ! dry_run )
				{
					SetResAttrs( code, resPurgeable );
					
					exit_status = (ChangedResource( code ), res_error())  ||
					              (UpdateResFile( resfile ), res_error());
				}
			}
		}
		
		CloseResFile( resfile );
	}
	
	return exit_status;
}
