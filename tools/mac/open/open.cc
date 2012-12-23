/*
	open.cc
	-------
*/

// Standard C/C++
#include <cstdio>

// Standard C
#include <stdlib.h>
#include <string.h>

// gear
#include "gear/quad.hh"

// Nitrogen
#include "Mac/Files/Types/FSCreator.hh"

#include "Nitrogen/AEInteraction.hh"
#include "Nitrogen/Aliases.hh"
#include "Nitrogen/AppleEvents.hh"
#include "Nitrogen/Processes.hh"
#include "Nitrogen/Str.hh"

// Io: MacFiles
#include "MacFiles/Classic.hh"

// poseven
#include "poseven/extras/slurp.hh"
#include "poseven/functions/open.hh"
#include "poseven/types/errno_t.hh"

// Nitrogen
#include "Mac/AppleEvents/Types/DescType_scribe_dynamic.hh"

// FindProcess
#include "FindProcess.hh"

// Divergence
#include "Divergence/Utilities.hh"

// Orion
#include "Orion/get_options.hh"
#include "Orion/Main.hh"


namespace tool
{
	
	namespace n = nucleus;
	namespace N = Nitrogen;
	namespace p7 = poseven;
	namespace NX = NitrogenExtras;
	namespace Div = Divergence;
	namespace o = orion;
	
	
	enum
	{
		sigFinder = 'MACS'
	};
	
	
	static bool gActivate = false;
	
	
	static FSSpec ResolvePathname( const char* pathname, bool macPathname )
	{
		return macPathname ? N::FSMakeFSSpec         ( N::Str255( pathname ) ) 
		                   : Div::ResolvePathToFSSpec(            pathname   );
	}
	
	static inline n::owned< Mac::AEDesc_Data > AECoerce_Alias_From_FSSpec( const FSSpec& item )
	{
		return N::AECoercePtr< Mac::typeFSS >( item, Mac::typeAlias );
	}
	
	static n::owned< Mac::AEDesc_Data > CoerceFSSpecToAliasDesc( const FSSpec& item )
	{
		if ( TARGET_API_MAC_CARBON )
		{
			// Don't catch errAECoercionFail; it shouldn't happen
			return AECoerce_Alias_From_FSSpec( item );
		}
		
		try
		{
			return AECoerce_Alias_From_FSSpec( item );
		}
		catch ( const Mac::OSStatus& err )
		{
			if ( err != errAECoercionFail )
			{
				throw;
			}
		}
		
		// Older systems don't provide alias->FSSpec coercion, so do it manually.
		
		return N::AECreateDesc( Mac::typeAlias, N::NewAlias( item ) );
	}
	
	static n::owned< Mac::AppleEvent > MakeOpenDocsEvent( const Mac::AEDescList_Data&  items,
	                                                      const ProcessSerialNumber&   psn )
	{
		n::owned< Mac::AppleEvent > appleEvent = N::AECreateAppleEvent( Mac::kCoreEventClass,
		                                                                Mac::kAEOpenDocuments,
		                                                                N::AECreateDesc< Mac::typeProcessSerialNumber >( psn ) );
		
		N::AEPutParamDesc( appleEvent, Mac::keyDirectObject, items );
		
		return appleEvent;
	}
	
	static void OpenItemsWithRunningApp( const Mac::AEDescList_Data&  items,
	                                     const ProcessSerialNumber&   psn )
	{
		if ( gActivate )
		{
			N::SetFrontProcess( psn );
		}
		
		N::AESend( MakeOpenDocsEvent( items, psn ),
		           Mac::kAENoReply | Mac::kAECanInteract );
		
		if ( gActivate )
		{
			while ( !N::SameProcess( psn, N::GetFrontProcess() ) )
			{
				sleep( 0 );
			}
		}
	}
	
	static void LaunchApplicationWithDocsToOpen( const FSSpec&                app,
	                                             const Mac::AEDescList_Data&  items )
	{
		std::auto_ptr< AppParameters > appParameters
			= N::AEGetDescData< Mac::typeAppParameters >( N::AECoerceDesc( MakeOpenDocsEvent( items, N::NoProcess() ),
		                                                                   Mac::typeAppParameters ) );
		
		N::LaunchApplication( app, N::LaunchFlags(), appParameters.get() );
	}
	
	
	static const char* gAppNameToOpenIn = NULL;
	static const char* gAppSigToOpenIn  = NULL;
	
	static bool gOpenInEditor    = false;
	static bool gUseMacPathnames = false;
	
	static void DefineOptions()
	{
		o::bind_option_to_variable( "--app",  gAppNameToOpenIn );
		o::bind_option_to_variable( "--sig",  gAppSigToOpenIn  );
		o::bind_option_to_variable( "--mac",  gUseMacPathnames );
		o::bind_option_to_variable( "--edit", gOpenInEditor    );
		o::bind_option_to_variable( "--actv", gActivate        );
		
		o::alias_option( "--app",  "-a" );
		o::alias_option( "--edit", "-e" );
		o::alias_option( "--edit", "-t" );
		
		o::alias_option( "--actv", "--activate" );
	}
	
	static Mac::FSCreator DefaultTextFileCreator()
	{
	#ifdef __RELIX__
		
		const char* path = "/sys/type/text/.~DEFAULT";
		
		const plus::string code = p7::slurp( p7::open( path, p7::o_rdonly ) );
		
		return Mac::FSCreator( gear::decode_quad( code.data() ) );
		
	#else
		
		return Mac::FSCreator( 'ttxt' );  // for OS X
		
	#endif
	}
	
	static Mac::FSCreator SignatureOfAppForOpening()
	{
		if ( gOpenInEditor )
		{
			// User has specified default text editor.
			// Check MAC_EDITOR_SIGNATURE environment variable first.
			// If set, it must be a four-character code.
			
			if ( const char* macEditorSignature = getenv( "MAC_EDITOR_SIGNATURE" ) )
			{
				if ( strlen( macEditorSignature ) == sizeof 'quad' )
				{
					return Mac::FSCreator( gear::decode_quad( macEditorSignature ) );
				}
				
				// Treat a malformed quad value the same as no value.  Move on.
			}
			
			// No MAC_EDITOR_SIGNATURE; use the MacRelix default.
			
			return DefaultTextFileCreator();
		}
		
		if ( gAppSigToOpenIn != NULL )
		{
			// User has specified an application by its signature
			
			if ( strlen( gAppSigToOpenIn ) != sizeof 'quad' )
			{
				Mac::ThrowOSStatus( paramErr );
			}
			
			return Mac::FSCreator( gear::decode_quad( gAppSigToOpenIn ) );
		}
		
		// Otherwise, give everything to the Finder.
		
		return Mac::FSCreator( sigFinder );
	}
	
	static void OpenItemsUsingOptions( const Mac::AEDescList_Data& items )
	{
		// we either have a pathname or signature for the app.
		// if pathname, resolve to FSSpec and check if it's running.
		// if running, send AE and return
		// if signature, check if it's running.
		// if it's running, send it odoc and return
		// if not, lookup in DT
		// launch with app parameters
		
		// ways to specify app:
		// --app: pathname
		// --sig: signature
		// -e or -t: either?
		// default: sig
		
		FSSpec appFile;
		
		if ( gAppNameToOpenIn != NULL )
		{
			// User has specified an application by its pathname
			
			// Resolve to FSSpec
			appFile = Div::ResolvePathToFSSpec( gAppNameToOpenIn );
			
			try
			{
				// Find it if running
				ProcessSerialNumber psn = NX::FindProcess( appFile );
				
				// The app is already running -- send it an odoc event
				OpenItemsWithRunningApp( items, psn );
				
				// We're done
				return;
			}
			catch ( const Mac::OSStatus& err )
			{
				if ( err != procNotFound )
				{
					throw;
				}
				
				// No such process
				// appFile is already set
			}
		}
		else
		{
			// Look up by signature.
			
			// Pick a signature
			Mac::FSCreator signature = SignatureOfAppForOpening();
			
			try
			{
				// Find it if running
				ProcessSerialNumber psn = NX::FindProcess( signature );
				
				// The app is already running -- send it an odoc event
				OpenItemsWithRunningApp( items, psn );
				
				// We're done
				return;
			}
			catch ( const Mac::OSStatus& err )
			{
				if ( err != procNotFound )
				{
					throw;
				}
				
				// No such process
				appFile = N::DTGetAPPL( signature );
			}
		}
		
		LaunchApplicationWithDocsToOpen( appFile, items );
	}
	
	int Main( int argc, char** argv )
	{
		DefineOptions();
		
		o::get_options( argc, argv );
		
		char const *const *free_args = o::free_arguments();
		
		n::owned< Mac::AEDescList_Data > items = N::AECreateList< Mac::AEDescList_Data >( false );
		
		for ( char const *const *it = free_args;  *it != NULL;  ++it )
		{
			const char* pathname = *it;
			
			try
			{
				FSSpec item = ResolvePathname( pathname, gUseMacPathnames );
				
				if ( !io::item_exists( item ) )
				{
					throw p7::errno_t( ENOENT );
				}
				
				N::AEPutDesc( items, 0, CoerceFSSpecToAliasDesc( item ) );
			}
			catch ( const Mac::OSStatus& err )
			{
				std::fprintf( stderr, "open: %s: OSStatus %ld\n", pathname, err.Get() );
			}
			catch ( const p7::errno_t& errnum )
			{
				std::fprintf( stderr, "open: %s: %s\n", pathname, std::strerror( errnum ) );
			}
		}
		
		OpenItemsUsingOptions( items );
		
		return 0;
	}

}

