/*
	postlink.cc
	-----------
*/

// iota
#include "iota/strings.hh"

// more-libc
#include "more/string.h"

// poseven
#include "poseven/functions/open.hh"
#include "poseven/functions/truncate.hh"
#include "poseven/functions/write.hh"

// Nitrogen
#include "Nitrogen/Resources.hh"

// Divergence
#include "Divergence/Utilities.hh"

// Orion
#include "Orion/get_options.hh"
#include "Orion/Main.hh"


namespace tool
{
	
	namespace N = Nitrogen;
	namespace n = nucleus;
	namespace p7 = poseven;
	namespace Div = Divergence;
	namespace o = orion;
	
	
	static bool Patch68KStartupCode( ::Handle code )
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
		*setupMainRsrc = *jmpToMain + 8;
		*restoreRegs   = nopnop;
		*jmpToMain     = nopnop;
		
		return true;
	}
	
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
		bool dry_run = false;
		bool verbose = false;
		bool in_data = false;
		
		const char* link_map_path = NULL;
		
		o::bind_option_to_variable( "-n", dry_run );
		o::bind_option_to_variable( "-v", verbose );
		
		o::bind_option_to_variable( "--data-fork", in_data );
		
		o::get_options( argc, argv );
		
		char const *const *free_args = o::free_arguments();
		
		const std::size_t n_args = o::free_argument_count();
		
		if ( n_args == 0 )
		{
			p7::write( p7::stderr_fileno, STR_LEN( "postlink: argument required\n" ) );
			
			return 1;
		}
		
		const char* target_path = free_args[ 0 ];
		
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

