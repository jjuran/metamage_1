/*	=======
 *	open.cc
 *	=======
 */

// C Standard Library
#include <stdlib.h>
#include <string.h>

// C Standard Library
#include <string.h>

// iota
#include "iota/quad.hh"

// Nitrogen
#include "Nitrogen/AEInteraction.hh"
#include "Nitrogen/AERegistry.hh"
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
	
	static inline n::owned< N::AEDesc_Data > AECoerce_Alias_From_FSSpec( const FSSpec& item )
	{
		return N::AECoercePtr< N::typeFSS >( item, N::typeAlias );
	}
	
	static n::owned< N::AEDesc_Data > CoerceFSSpecToAliasDesc( const FSSpec& item )
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
		catch ( const N::OSStatus& err )
		{
			if ( err != errAECoercionFail )
			{
				throw;
			}
		}
		
		// Older systems don't provide alias->FSSpec coercion, so do it manually.
		
		return N::AECreateDesc( N::typeAlias, N::NewAlias( item ) );
	}
	
	static n::owned< N::AppleEvent > MakeOpenDocsEvent( const N::AEDescList_Data&   items,
	                                                    const ProcessSerialNumber&  psn )
	{
		n::owned< N::AppleEvent > appleEvent = N::AECreateAppleEvent( N::kCoreEventClass,
		                                                              N::kAEOpenDocuments,
		                                                              N::AECreateDesc< N::typeProcessSerialNumber >( psn ) );
		
		N::AEPutParamDesc( appleEvent, N::keyDirectObject, items );
		
		return appleEvent;
	}
	
	static void OpenItemsWithRunningApp( const N::AEDescList_Data&   items,
	                                     const ProcessSerialNumber&  psn )
	{
		if ( gActivate )
		{
			N::SetFrontProcess( psn );
		}
		
		N::AESend( MakeOpenDocsEvent( items, psn ),
		           N::kAENoReply | N::kAECanInteract );
		
		if ( gActivate )
		{
			while ( !N::SameProcess( psn, N::GetFrontProcess() ) )
			{
				sleep( 0 );
			}
		}
	}
	
	static void LaunchApplicationWithDocsToOpen( const FSSpec&              app,
	                                             const N::AEDescList_Data&  items )
	{
		std::auto_ptr< AppParameters > appParameters
			= N::AEGetDescData< N::typeAppParameters >( N::AECoerceDesc( MakeOpenDocsEvent( items, N::NoProcess() ),
		                                                                 N::typeAppParameters ) );
		
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
	
	static N::OSType DefaultTextFileCreator()
	{
	#ifdef __LAMP__
		
		const char* path = "/sys/type/text/DEFAULT";
		
		const plus::string code = p7::slurp( p7::open( path, p7::o_rdonly | p7::o_binary ) );
		
		return N::OSType( iota::decode_quad( code.data() ) );
		
	#else
		
		return N::OSType( 'ttxt' );  // for OS X
		
	#endif
	}
	
	static N::OSType SignatureOfAppForOpening()
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
					return N::OSType( iota::decode_quad( macEditorSignature ) );
				}
				
				// Treat a malformed quad value the same as no value.  Move on.
			}
			
			// No MAC_EDITOR_SIGNATURE; use the Lamp default.
			
			return DefaultTextFileCreator();
		}
		
		if ( gAppSigToOpenIn != NULL )
		{
			// User has specified an application by its signature
			
			if ( strlen( gAppSigToOpenIn ) != sizeof 'quad' )
			{
				N::ThrowOSStatus( paramErr );
			}
			
			return N::OSType( iota::decode_quad( gAppSigToOpenIn ) );
		}
		
		// Otherwise, give everything to the Finder.
		
		return N::OSType( sigFinder );
	}
	
	static void OpenItemsUsingOptions( const N::AEDescList_Data& items )
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
			catch ( const N::OSStatus& err )
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
			N::OSType signature = SignatureOfAppForOpening();
			
			try
			{
				// Find it if running
				ProcessSerialNumber psn = NX::FindProcess( signature );
				
				// The app is already running -- send it an odoc event
				OpenItemsWithRunningApp( items, psn );
				
				// We're done
				return;
			}
			catch ( const N::OSStatus& err )
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
	
	int Main( int argc, iota::argv_t argv )
	{
		DefineOptions();
		
		o::get_options( argc, argv );
		
		char const *const *free_args = o::free_arguments();
		
		n::owned< N::AEDescList_Data > items = N::AECreateList< N::AEDescList_Data >( false );
		
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
			catch ( const N::OSStatus& err )
			{
				std::fprintf( stderr, "open: %s: OSStatus %d\n", pathname, err.Get() );
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

