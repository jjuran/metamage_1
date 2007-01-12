/*	=======
 *	open.cc
 *	=======
 */

// Universal Interfaces
#ifndef __AEREGISTRY__
#include <AERegistry.h>
#endif

// C Standard Library
#include <stdlib.h>

// C++ Standard Library
#include <functional>
#include <string>
#include <vector>

// Nitrogen
#include "Nitrogen/AEInteraction.h"
#include "Nitrogen/Str.h"

// Nitrogen Extras / Iteration
#include "Iteration/AEDescListItems.h"

// Nitrogen Extras / Operators
#include "Operators/AEDataModel.h"

// Nitrogen Extras / Utilities
#include "Utilities/Files.h"
#include "Utilities/Processes.h"

// Divergence
#include "Divergence/Utilities.hh"

// Orion
#include "Orion/GetOptions.hh"
#include "Orion/Main.hh"


namespace N = Nitrogen;
namespace NN = Nucleus;
namespace NX = NitrogenExtras;
namespace Div = Divergence;
namespace O = Orion;


enum
{
	sigFinder       = 'MACS',
	sigBBEdit       = 'R*ch',
	sigTextWrangler = '!Rch',
	
	// FIXME
	sigGoodTextEditor = TARGET_API_MAC_CARBON ? sigTextWrangler : sigBBEdit
};

static FSSpec ResolvePathname( const std::string& pathname, bool macPathname )
{
	return macPathname ? N::FSMakeFSSpec         ( N::Str255( pathname       ) ) 
	                   : Div::ResolvePathToFSSpec(            pathname.c_str() );
}

static NN::Owned< N::AEDesc > CoerceFSSpecToAliasDesc( const FSSpec& item )
{
	return N::AECoercePtr< N::typeFSS >( item, N::typeAlias );
}

static NN::Owned< N::AppleEvent > MakeOpenDocsEvent( const std::vector< FSSpec >&  items,
                                                     const ProcessSerialNumber&    psn )
{
	NN::Owned< N::AEDescList > documents = N::AECreateList< false >();
	
	std::transform( items.begin(),
	                items.end(),
	                N::AEDescList_Item_BackInserter( documents ),
	                std::ptr_fun( CoerceFSSpecToAliasDesc ) );
	
	using namespace NN::Operators;
	
	return N::AECreateAppleEvent( N::kCoreEventClass,
	                              N::kAEOpenDocuments,
	                              N::AECreateDesc< N::typeProcessSerialNumber >( psn ) )
	       << N::keyDirectObject + documents;
}

static void OpenItemsWithRunningApp( const std::vector< FSSpec >& items, const ProcessSerialNumber& psn )
{
	N::AESend( MakeOpenDocsEvent( items, psn ),
	           N::kAENoReply | N::kAECanInteract );
}

static void LaunchApplicationWithDocsToOpen( const FSSpec& app, const std::vector< FSSpec >& items )
{
	std::auto_ptr< AppParameters > appParameters
		= N::AEGetDescData< N::typeAppParameters >( N::AECoerceDesc( MakeOpenDocsEvent( items, N::NoProcess() ),
	                                                                 N::typeAppParameters ) );
	
	N::LaunchApplication( app, LaunchFlags(), appParameters.get() );
}


enum
{
	optOpenInApp,
	optOpenInAppWithSignature,
	optInBackground,
	optOpenInEditor,
	optInterpretMacPathnames
};

static O::Options DefineOptions()
{
	O::Options options;
	
	options.DefineSetString( "--app", optOpenInApp );
	options.DefineSetString( "-a",    optOpenInApp );
	
	options.DefineSetString( "--sig", optOpenInAppWithSignature );
	
	options.DefineSetFlag(   "-e",    optOpenInEditor );
	options.DefineSetFlag(   "-t",    optOpenInEditor );
	
	options.DefineSetFlag(   "--mac", optInterpretMacPathnames );
	
	return options;
}

static N::OSType SignatureOfAppForOpening( const O::Options& options )
{
	if ( options.GetFlag( optOpenInEditor ) )
	{
		// User has specified default text editor.
		// Check MAC_EDITOR_SIGNATURE environment variable first.
		// If set, it must be a four-character code.
		
		if ( const char* macEditorSignature = getenv( "MAC_EDITOR_SIGNATURE" ) )
		{
			return NN::Convert< N::OSType, std::string >( macEditorSignature );
		}
		
		// No MAC_EDITOR_SIGNATURE; default to BBEdit/TextWrangler.
		// FIXME:  We could be smarter about this...
		
		return N::OSType( sigGoodTextEditor );
	}
	
	const std::string appSignatureArg = options.GetString( optOpenInAppWithSignature );
	
	if ( !appSignatureArg.empty() )
	{
		// User has specified an application by its signature
		return NN::Convert< N::OSType >( appSignatureArg );
	}
	
	// Otherwise, give everything to the Finder.
	
	return N::OSType( sigFinder );
}

static void OpenItemsUsingOptions( const std::vector< FSSpec >& items, const O::Options& options )
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
	
	const std::string appPathname = options.GetString( optOpenInApp );
	
	FSSpec appFile;
	
	if ( !appPathname.empty() )
	{
		// User has specified an application by its pathname
		
		// Resolve to FSSpec
		appFile = Div::ResolvePathToFSSpec( appPathname.c_str() );
		
		try
		{
			// Find it if running
			ProcessSerialNumber psn = NX::FindProcess( appFile );
			
			// The app is already running -- send it an odoc event
			OpenItemsWithRunningApp( items, psn );
			
			// We're done
			return;
		}
		catch ( const N::ProcNotFound& )
		{
			// No such process
			// appFile is already set
		}
	}
	else
	{
		// Look up by signature.
		
		// Pick a signature
		N::OSType signature = SignatureOfAppForOpening( options );
		
		try
		{
			// Find it if running
			ProcessSerialNumber psn = NX::FindProcess( signature );
			
			// The app is already running -- send it an odoc event
			OpenItemsWithRunningApp( items, psn );
			
			// We're done
			return;
		}
		catch ( const N::ProcNotFound& )
		{
			// No such process
			appFile = NX::DTGetAPPL( signature );
		}
	}
	
	LaunchApplicationWithDocsToOpen( appFile, items );
}

int O::Main( int argc, char const *const argv[] )
{
	O::Options options = DefineOptions();
	options.GetOptions( argc, argv );
	
	const std::vector< const char* >& params = options.GetFreeParams();
	
	bool useMacPathnames = options.GetFlag( optInterpretMacPathnames );
	
	std::vector< FSSpec > itemsToOpen;
	
	typedef std::vector< const char* >::const_iterator const_iterator;
	
	for ( const_iterator it = params.begin();  it != params.end();  ++it )
	{
		const char* pathname = *it;
		
		try
		{
			FSSpec item = ResolvePathname( pathname, useMacPathnames );
			
			itemsToOpen.push_back( item );
		}
		catch ( ... )
		{
			// do nothing at the moment
		}
	}
	
	OpenItemsUsingOptions( itemsToOpen, options );
	
	return 0;
}

