/*
	postlink.cc
	-----------
*/

// Standard C
#include <stdlib.h>

// iota
#include "iota/strings.hh"

// more-libc
#include "more/string.h"

// command
#include "command/get_option.hh"

// plus
#include "plus/string.hh"

// poseven
#include "poseven/functions/open.hh"
#include "poseven/functions/truncate.hh"
#include "poseven/functions/write.hh"

// Nitrogen
#include "Nitrogen/Resources.hh"

// Divergence
#include "Divergence/Utilities.hh"

// Orion
#include "Orion/Main.hh"


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
	
	namespace N = Nitrogen;
	namespace n = nucleus;
	namespace p7 = poseven;
	namespace Div = Divergence;
	
	
	static n::owned< N::Handle > Patch68KStartup( const FSSpec& file )
	{
		N::ResType  resType = N::ResType( 'Tool' );
		N::ResID    resID   = N::ResID  ( 0      );
		
		n::owned< N::ResFileRefNum > resFile = N::FSpOpenResFile( file, Mac::fsRdWrPerm );
		
		N::Handle code = N::Get1Resource( resType, resID );
		
		const bool patched = Patch68KStartupCode( code.Get() );
		
		N::ChangedResource( code );
		
		N::WriteResource( code );
		
		return N::DetachResource( code );
	}
	
	
	int Main( int argc, char** argv )
	{
		char *const *args = get_options( argv );
		
		const int argn = argc - (args - argv);
		
		if ( argn == 0 )
		{
			p7::write( p7::stderr_fileno, STR_LEN( "postlink: argument required\n" ) );
			
			return 1;
		}
		
		const char* target_path = args[ 0 ];
		
		FSSpec target_filespec = Div::ResolvePathToFSSpec( target_path );
		
		if ( dry_run )
		{
			return 0;
		}
		
		n::owned< N::Handle > code = Patch68KStartup( target_filespec );
		
		// System calls can move memory, so just lock the handle
		N::HLock( code );
		
		if ( in_data )
		{
			p7::write( p7::open( target_path, p7::o_wronly ),
			           *code.get().Get(),
			           N::GetHandleSize( code ));
			
			code.reset();
			
			plus::string rsrc_path;
			
			const size_t len = strlen( target_path );
			
			char* p = rsrc_path.reset( len + STRLEN( "/rsrc" ) );
			
			const char* begin = p;
			
			p = (char*) mempcpy( p, target_path, len );
			p = (char*) mempcpy( p, STR_LEN( "/rsrc" ) );
			
			p7::truncate( begin, 0 );
		}
		
		return 0;
	}
	
}
