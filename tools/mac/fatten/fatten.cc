/*
	fatten.cc
	---------
*/

// Mac OS X
#ifdef __APPLE__
#include <CoreServices/CoreServices.h>
#include <copyfile.h>
#endif

// Mac OS
#ifndef __RESOURCES__
#include <Resources.h>
#endif

// POSIX
#include <fcntl.h>
#include <unistd.h>

// Standard C
#include <stdio.h>
#include <stdlib.h>

// Relix
#ifdef __RELIX__
#include "relix/copyfile.h"
#endif
#include "relix/pump.h"

// mac-rsrc-utils
#include "mac_rsrc/open_res_file.hh"

// mac-relix-utils
#include "mac_relix/FSRef_from_path.hh"
#include "mac_relix/FSSpec_from_path.hh"

// command
#include "command/get_option.hh"


#pragma exceptions off


#define PROGRAM "fatten"

#define STR_LEN(s)  "" s, (sizeof s - 1)


using namespace command::constants;

enum
{
	Option_68K_input = 'K',
	Option_PPC_input = 'P',
	Option_output    = 'o',
};

static command::option options[] =
{
	{ "--68K", Option_68K_input, Param_required },
	{ "--68k", Option_68K_input, Param_required },
	{ "--PPC", Option_PPC_input, Param_required },
	{ "--ppc", Option_PPC_input, Param_required },
	{ "--out", Option_output,    Param_required },
	
	{ NULL }
};

static const char* input_68k = "";
static const char* input_ppc = "";
static const char* output    = "";

static
char* const* get_options( char* const* argv )
{
	++argv;  // skip arg 0
	
	short opt;
	
	while ( (opt = command::get_option( &argv, options )) )
	{
		switch ( opt )
		{
			case Option_68K_input:
				input_68k = command::global_result.param;
				break;
			
			case Option_PPC_input:
				input_ppc = command::global_result.param;
				break;
			
			case Option_output:
				output = command::global_result.param;
				break;
			
			default:
				abort();
		}
	}
	
	return argv;
}

#if __LP64__

typedef FSRef FSObj;

#else

typedef FSSpec FSObj;

#endif

int main( int argc, char** argv )
{
	char *const *args = get_options( argv );
	
	const int argn = argc - (args - argv);
	
	char const *const *free_args = args;
	
	using mac::sys::Error;
	
	FSObj src, dst;
	
	Error err;
	
	/*
		Verify that both source files exist.  Having done so,
		we only need to keep an FSObj for the PPC input file,
		because we open it with the Resource Manager below.
	*/
	
	err = mac::relix::FSObj_from_existing_path( input_68k, src );
	
	if ( err )
	{
		perror( input_68k );
	}
	
	err = mac::relix::FSObj_from_existing_path( input_ppc, src );
	
	if ( err )
	{
		perror( input_ppc );
	}
	
#ifdef __APPLE__
	
	copyfile_state_t state = copyfile_state_alloc();
	
	int nok = copyfile( input_68k, output, state, COPYFILE_METADATA );
	
	copyfile_state_free( state );
	
#else
	
	int nok = copyfile( input_68k, output );
	
#endif
	
	if ( nok )
	{
		perror( output );
		
		return 1;
	}
	
	err = mac::relix::FSObj_from_existing_path( output, dst );
	
	if ( err )
	{
		perror( output );
	}
	
	int in  = open( input_ppc, O_RDONLY );
	int out = open( output,    O_WRONLY | O_TRUNC );
	
	if ( in > 0  &&  out > 0 )
	{
		ssize_t pumped = pump( in, NULL, out, NULL, 0, 0 );
		
		if ( pumped < 0 )
		{
			perror( "pump" );
		}
	}
	
	close( in  );
	close( out );
	
	short resfile = mac::rsrc::open_res_file( src, fsRdPerm );
	
	if ( resfile > 0 )
	{
		Handle h;
		
		if ( (h = Get1Resource( 'cfrg', 0 )) )
		{
			DetachResource( h );
		}
		
		CloseResFile( resfile );
		
		if ( h )
		{
			resfile = mac::rsrc::open_res_file( dst, fsRdWrPerm );
			
			if ( resfile > 0 )
			{
				AddResource( h, 'cfrg', 0, "\p" );
				
				CloseResFile( resfile );
			}
			else
			{
				DisposeHandle( h );
			}
		}
	}
	
	return 0;
}
