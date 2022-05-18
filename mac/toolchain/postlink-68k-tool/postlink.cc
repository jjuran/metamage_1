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

// POSIX
#include <unistd.h>

// Standard C
#include <stdlib.h>

// iota
#include "iota/strings.hh"

// command
#include "command/get_option.hh"

// mac-relix-utils
#include "mac_relix/FSSpec_from_path.hh"

// Orion
#include "Orion/Main.hh"


#pragma exceptions off


#define PREFIX  "postlink-68k-tool: "

#define ERROR( e, msg )  (wrote( 2, STR_LEN( PREFIX msg "\n" ) ) ? e : 13)


using namespace command::constants;

enum
{
	Option_dry_run = 'n',
	Option_verbose = 'v',
	
	Option_last_byte = 255,
	
	Option_data_fork,
};

static command::option options[] =
{
	{ "data-fork", Option_data_fork },
	
	{ "", Option_dry_run },
	{ "", Option_verbose },
	
	{ NULL }
};

static bool dry_run = false;
static bool verbose = false;
static bool in_data = false;

static inline
bool wrote( int fd, const void* buffer, size_t n )
{
	return write( fd, buffer, n ) == n;
}

static char* const* get_options( char* const* argv )
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
			
			case Option_data_fork:
				in_data = true;
				break;
			
			default:
				abort();
		}
	}
	
	return argv;
}

static
bool Patch68KStartupCode( Handle code )
{
	const UInt32 nopnop = 0x4e714e71;
	const UInt32 jmp    = 0x4efa0000;
	
	UInt32* const saveRegisters = (UInt32*) (*code + 12);
	UInt32* const setCurrentA4  = (UInt32*) (*code + 16);
	UInt32* const loadStartToA0 = (UInt32*) (*code + 20);
	UInt32* const moveAndStrip  = (UInt32*) (*code + 24);
	UInt32* const setupMainRsrc = (UInt32*) (*code + 28);
	UInt32* const restoreRegs   = (UInt32*) (*code + 32);
	UInt32* const jmpToMain     = (UInt32*) (*code + 36);
	
	const bool is_far = *moveAndStrip == 0x2008A055;
	
	if ( ! is_far )
	{
		return true;
	}
	
	if ( *jmpToMain == nopnop )
	{
		return false;
	}
	
	*saveRegisters = *setCurrentA4  + 4;
	*setCurrentA4  = *loadStartToA0 + 4;
	*loadStartToA0 = *moveAndStrip;
	*moveAndStrip  = *setupMainRsrc + 4;
	*setupMainRsrc = *jmpToMain + 8;
	*restoreRegs   = nopnop;
	*jmpToMain     = nopnop;
	
	return true;
}


namespace tool
{
	
	static
	Handle Patch68KStartup()
	{
		Handle code = Get1Resource( 'Tool', 0 );
		
		if ( code )
		{
			if ( const bool patched = Patch68KStartupCode( code ) )
			{
				ChangedResource( code );
				WriteResource  ( code );
			}
		}
		
		return code;
	}
	
	
	int Main( int argc, char** argv )
	{
		char *const *args = get_options( argv );
		
		const int argn = argc - (args - argv);
		
		if ( argn == 0 )
		{
			ERROR( 50, "argument required" );
		}
		
		const char* target_path = args[ 0 ];
		
		FSSpec target_filespec;
		if ( mac::relix::FSSpec_from_existing_path( target_path, target_filespec ) )
		{
			return 1;
		}
		
		if ( dry_run )
		{
			return 0;
		}
		
		short resfile = FSpOpenResFile( &target_filespec, fsRdWrPerm );
		
		if ( resfile <= 0 )
		{
			return 1;
		}
		
		Handle code = Patch68KStartup();
		
		OSErr err;
		
		if ( in_data )
		{
			short refnum;
			
			err = FSpOpenDF( &target_filespec, fsRdWrPerm, &refnum );
			
			if ( err == noErr )
			{
				SInt32 size = GetHandleSize( code );
				
				err = FSWrite( refnum, &size, *code );
				
				OSErr err2 = FSClose( refnum );
				
				if ( err == noErr )
				{
					err = err2;
				}
			}
		}
		
		CloseResFile( resfile );
		
		if ( in_data  &&  err == noErr )
		{
			short refnum;
			
			err = FSpOpenRF( &target_filespec, fsRdWrPerm, &refnum );
			
			if ( err == noErr )
			{
				err = SetEOF( refnum, 0 );
				
				OSErr err2 = FSClose( refnum );
				
				if ( err == noErr )
				{
					err = err2;
				}
			}
		}
		
		return err > -125 ? -err : 125;
	}
	
}
