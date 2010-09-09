/*
	postlink.cc
	-----------
*/

// Standard C/C++
#include <cstdio>

// Standard C
#include <stdlib.h>
#include <string.h>

// Lamp
#include "lamp/alloca.h"

// iota
#include "iota/strings.hh"

// poseven
#include "poseven/functions/unlink.hh"
#include "poseven/functions/write.hh"
#include "poseven/types/exit_t.hh"

// Nitrogen
#include "Nitrogen/Resources.hh"

// Divergence
#include "Divergence/Utilities.hh"

// Orion
#include "Orion/get_options.hh"
#include "Orion/Main.hh"

// postlink
#include "link_map.hh"


namespace tool
{
	
	namespace N = Nitrogen;
	namespace n = nucleus;
	namespace p7 = poseven;
	namespace Div = Divergence;
	namespace o = orion;
	
	
	static bool Patch68KStartupCode( ::Handle code, UInt32 lampMainOffset )
	{
		const UInt32 nopnop = 0x4e714e71;
		const UInt32 jmp    = 0x4efa0000;
		
		UInt32* const saveRegisters = reinterpret_cast< UInt32* >( *code + 12 );
		UInt32* const setCurrentA4  = reinterpret_cast< UInt32* >( *code + 16 );
		UInt32* const loadStartToA0 = reinterpret_cast< UInt32* >( *code + 20 );
		UInt32* const moveAndStrip  = reinterpret_cast< UInt32* >( *code + 24 );
		UInt32* const setupMainRsrc = reinterpret_cast< UInt32* >( *code + 28 );
		UInt32* const restoreRegs   = reinterpret_cast< UInt32* >( *code + 32 );
		UInt32* const jmpToMain     = reinterpret_cast< UInt32* >( *code + 36 );
		
		if ( *jmpToMain == nopnop )
		{
			return false;
		}
		
		*saveRegisters = *setCurrentA4  + 4;
		*setCurrentA4  = *loadStartToA0 + 4;
		*loadStartToA0 = *moveAndStrip;
		*moveAndStrip  = *setupMainRsrc + 4;
		*setupMainRsrc = lampMainOffset ? jmp | (lampMainOffset - 28 - 2) : *jmpToMain + 8;
		*restoreRegs   = nopnop;
		*jmpToMain     = nopnop;
		
		return true;
	}
	
	static bool Patch68KStartup( const char* path, const FSSpec& file )
	{
		const unsigned long lampmain = get_code_offset( "_lamp_main" );
		
		if ( lampmain > 0x7fff )
		{
			std::fprintf( stderr, "ld: _lamp_main() offset 0x%.8x is out of range for 16-bit reference\n", lampmain );
			
			p7::unlink( path );
			
			throw p7::exit_failure;
		}
		
		N::ResType  resType = N::ResType( 'Wish' );
		N::ResID    resID   = N::ResID  ( 0      );
		
		n::owned< N::ResFileRefNum > resFile = N::FSpOpenResFile( file, Mac::fsRdWrPerm );
		
		N::Handle code = N::Get1Resource( resType, resID );
		
		const bool patched = Patch68KStartupCode( code.Get(), lampmain );
		
		N::ChangedResource( code );
		
		N::WriteResource( code );
		
		return patched;
	}
	
	
	int Main( int argc, char** argv )
	{
		bool dry_run = false;
		bool verbose = false;
		
		const char* link_map_path = NULL;
		
		o::bind_option_to_variable( "-n", dry_run );
		o::bind_option_to_variable( "-v", verbose );
		
		o::bind_option_to_variable( "--map", link_map_path );
		
		o::get_options( argc, argv );
		
		char const *const *free_args = o::free_arguments();
		
		const std::size_t n_args = o::free_argument_count();
		
		if ( n_args == 0 )
		{
			p7::write( p7::stderr_fileno, STR_LEN( "postlink: argument required\n" ) );
			
			return EXIT_FAILURE;
		}
		
		const char* target_path = free_args[ 0 ];
		
		if ( link_map_path == NULL )
		{
			size_t target_path_len = strlen( target_path );
			
			char* stack = (char*) checked_alloca( target_path_len + sizeof ".map" );
			
			memcpy( stack, target_path, target_path_len );
			
			memcpy( stack + target_path_len, STR_LEN( ".map" ) + 1 );
			
			link_map_path = stack;
		}
		
		read_link_map( link_map_path );
		
		FSSpec target_filespec = Div::ResolvePathToFSSpec( target_path );
		
		if ( dry_run )
		{
			return 0;
		}
		
		const bool patched = Patch68KStartup( target_path, target_filespec );
		
		if ( !patched )
		{
			fprintf( stderr, "postlink: %s: already patched\n", target_path );
		}
		
		return 0;
	}
	
}

