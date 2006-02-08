/*	=======
 *	open.cc
 *	=======
 */

// Universal Interfaces
#ifndef __AEREGISTRY__
#include <AERegistry.h>
#endif

// C++ Standard Library
#include <string>

// Nitrogen
#include "Nitrogen/AEInteraction.h"
#include "Nitrogen/Str.h"

// Nitrogen Extras / Utilities
#include "Utilities/Files.h"
#include "Utilities/Processes.h"

// Orion
#include "Orion/GetOptions.hh"
#include "Orion/Main.hh"
#include "SystemCalls.hh"


namespace N = Nitrogen;
namespace NN = Nucleus;
namespace NX = NitrogenExtras;
namespace O = Orion;

using std::string;


enum
{
	sigFinder       = 'MACS',
	sigBBEdit       = 'R*ch',
	sigTextWrangler = '!Rch',
	
	// FIXME
	sigGoodTextEditor = TARGET_API_MAC_CARBON ? sigTextWrangler : sigBBEdit
};

static void OpenItemInRunningApp( const FSSpec& item, const ProcessSerialNumber& psn )
{
	N::AESend( N::AECreateAppleEvent( kCoreEventClass,
				                      kAEOpenDocuments,
				                      N::AECreateDesc< typeProcessSerialNumber >( psn ) )
			   << keyDirectObject + ( N::AECreateList< false >()
			                          << N::AECoercePtr< typeFSS >( item, typeAlias ) ),
			   kAEWaitReply | kAECanInteract );
	
}

static void OpenItemInApp( const FSSpec& item, N::OSType signature )
{
	// Make sure the app is running first
	OpenItemInRunningApp( item, NX::LaunchApplication( signature ) );
}

static void Open( const FSSpec& item )
{
	FInfo info;
	
	if ( N::FSpTestDirectoryExists( item ) )
	{
		// is a directory
		info.fdType = 'fold';
	}
	else
	{
		// is a file or doesn't exist; we'll soon find out which
		info = N::FSpGetFInfo( item );
	}
	
	switch ( info.fdType )
	{
		case 'APPL':
			ProcessSerialNumber psn = N::LaunchApplication( item );
			break;
		
		default:
			OpenItemInApp( item, sigFinder );
			break;
	}
}


enum
{
	optOpenInApp, 
	optOpenInAppWithSignature, 
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
	options.DefineSetFlag(   "--mac", optInterpretMacPathnames );
	
	return options;
}

class Opener
{
	private:
		const O::Options& options;
	
	public:
		Opener( const O::Options& options ) : options( options )  {}
		
		void operator()( const string& pathname ) const;
};

void Opener::operator()( const string& pathname ) const
{
	bool useMacPathnames = options.GetFlag( optInterpretMacPathnames );
	
	FSSpec item = useMacPathnames ? N::FSMakeFSSpec( N::Str255( pathname ) ) 
	                              : Path2FSS       ( pathname              );
	
	N::OSType signature;
	
	if ( options.GetFlag( optOpenInEditor ) )
	{
		signature = sigGoodTextEditor;
	}
	else
	{
		const std::string sigParam = options.GetString( optOpenInAppWithSignature );
		signature = NN::Convert< N::OSType >( sigParam );
	}
	
	if ( signature != N::OSType() )
	{
		OpenItemInApp( item, signature );
	}
	else
	{
		Open( item );
	}
}

int O::Main( int argc, char const *const argv[] )
{
	O::Options options = DefineOptions();
	options.GetOptions( argc, argv );
	
	const std::vector< const char* >& params = options.GetFreeParams();
	
	std::for_each( params.begin(), params.end(), Opener( options ) );
	
	return 0;
}

