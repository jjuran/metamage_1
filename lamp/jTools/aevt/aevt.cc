/*	=======
 *	aevt.cc
 *	=======
 */

// Mac OS
#include <TargetConditionals.h>
#if TARGET_RT_MAC_MACHO
	#include <Carbon/Carbon.h>
#else
	#include <AEHelpers.h>
	#include <Processes.h>
	#include <Script.h>
#endif

// Standard C++
#include <vector>

// Nitrogen
#include "Nitrogen/AEInteraction.h"
#include "Nitrogen/MacErrors.h"
#include "Nitrogen/Str.h"

#if CALL_NOT_IN_CARBON

// ClassicToolbox
#include "ClassicToolbox/AEDataModel.h"
#include "ClassicToolbox/AppleTalk.h"
#include "ClassicToolbox/EPPC.h"
#include "ClassicToolbox/PPCToolbox.h"

#endif

// Orion
#include "Orion/GetOptions.hh"
#include "Orion/Main.hh"
#include "Orion/StandardIO.hh"


namespace O = Orion;

namespace jTools
{
	
	namespace N = Nitrogen;
	namespace NN = Nucleus;
	
	
	static NN::Owned< AEDesc > BuildAppleEvent( AEEventClass          eventClass,
	                                            AEEventID             eventID,
	                                            const AEAddressDesc&  address,
	                                            const char*           buildString,
	                                            va_list               args,
	                                            AEReturnID            returnID      = kAutoGenerateReturnID,
	                                            AETransactionID       transactionID = kAnyTransactionID )
	{
		std::size_t addrSize = N::AEGetDescDataSize( address );
		
		std::vector< char > addrData( addrSize );
		
		N::AEGetDescData( address, &addrData.front(), addrSize );
		
		N::AEDesc appleEvent;
		AEBuildError aeErr;
		
		N::ThrowOSStatus( ::vAEBuildAppleEvent( eventClass,
		                                        eventID,
		                                        address.descriptorType,
		                                        &addrData.front(),
		                                        addrSize,
		                                        returnID,
		                                        transactionID,
		                                        &appleEvent,
		                                        &aeErr,
		                                        buildString,
		                                        args ) );
		
		return NN::Owned< AEDesc >::Seize( appleEvent );
	}
	
#if CALL_NOT_IN_CARBON
	
	static TargetID LocateTarget( const std::string& appName,
	                              const std::string& machine,
	                              const std::string& host )
	{
		PPCPortRec name = NN::Make< PPCPortRec >( N::Str32( appName ), "\p=" );
		
		/*
		LocationNameRec location = machine.empty() ? host.empty() ? NN::Make< LocationNameRec >()
		                                                          : NN::Make< LocationNameRec >( NN::Make< PPCAddrRec >( NN::Make< PPCXTIAddress >( host ) ) )
		                                           : NN::Make< LocationNameRec >( NN::Make< EntityName >( N::Str32( machine ), "\pPPCToolbox" ) );
		*/
		
		LocationNameRec location = !machine.empty() ? NN::Make< LocationNameRec >( NN::Make< EntityName >( N::Str32( machine ), "\pPPCToolbox" ) )
		                         : !host.empty()    ? NN::Make< LocationNameRec >( NN::Make< PPCAddrRec >( NN::Make< PPCXTIAddress >( host ) ) )
		                                            : NN::Make< LocationNameRec >();
		
		return NN::Make< TargetID >( N::IPCListPortsSync( name, location ).name, location );
	}
	
#endif
	
	static NN::Owned< AEDesc > SelectAddress( N::OSType           sig,
	                                          const std::string&  app,
	                                          const std::string&  machine,
	                                          const std::string&  host,
	                                          const std::string&  url )
	{
		if ( sig != kUnknownType )
		{
			return N::AECreateDesc< N::typeApplSignature >( sig );
		}
		else
		{
		#if CALL_NOT_IN_CARBON
			
			return N::AECreateDesc< N::typeTargetID >( LocateTarget( app, machine, host ) );
			
		#else
			
			return N::AECreateDesc< N::typeApplicationURL >( url );
			
		#endif
		}
		
		// Not reached
		return NN::Owned< AEDesc >();
	}
	
	// shell$ aevt -m Otter -a Genie |gan Exec '----':[“shutdown”,“-h”]
	// shell$ aevt -s hhgg aevt quit
	
	enum
	{
		optURL, 
		optHost, 
		optMachine, 
		optApplicationName, 
		optApplicationSignature
	};
	
	static O::Options DefineOptions()
	{
		O::Options options;
		
		options.DefineSetString( "-u", optURL );
		options.DefineSetString( "-h", optHost );
		options.DefineSetString( "-m", optMachine );
		options.DefineSetString( "-a", optApplicationName );
		options.DefineSetString( "-s", optApplicationSignature );
		
		options.DefineSetString( "--url", optURL );
		options.DefineSetString( "--host", optHost );
		options.DefineSetString( "--machine", optMachine );
		options.DefineSetString( "--appname", optApplicationName );
		options.DefineSetString( "--signature", optApplicationSignature );
		
		options.DefineSetString( "--app", optApplicationName );
		options.DefineSetString( "--sig", optApplicationSignature );
		
		return options;
	}
	
	static int Main( int argc, const char *const argv[] )
	{
		O::Options options = DefineOptions();
		options.GetOptions( argc, argv );
		
		const std::vector< const char* >& params = options.GetFreeParams();
		
		std::string argBuild, argEventClass, argEventID;
		
		if ( params.size() == 0 )
		{
			Io::Err << 
				"Usage:  aevt [-m machine] {-a app | -s sign} class id [params]\n"
				"Examples: aevt -s hhgg aevt quit\n"
				"          aevt -m 'Headless Mac' -a Genie |gan Exec \"'----':[“shutdown -h”]\"\n"
				"          aevt -s R*ch misc slct \"'----':obj{want:type(clin), form:indx, seld:42,\n"
				"               from:obj{want:type(cwin), form:indx, seld:1, from:null()}}\"\n";
			return 0;
		}
		else if ( params.size() < 2 )
		{
			Io::Err << "aevt: missing arguments" "\n";
			return 1;
		}
		else if ( params.size() < 3 )
		{
			argBuild = "";
		}
		else
		{
			argBuild = params[2];
		}
		
		argEventClass = params[0];
		argEventID    = params[1];
		
		if ( argEventClass.size() != 4  ||  argEventID.size() != 4 )
		{
			Io::Err << "aevt: invalid parameter" "\n";
			
			return 1;
		}
		
		std::string url     = options.GetString( optURL );
		std::string host    = options.GetString( optHost );
		std::string machine = options.GetString( optMachine );
		std::string app     = options.GetString( optApplicationName );
		std::string sig     = options.GetString( optApplicationSignature );
		
		N::OSType sigCode = (sig.size() == 4) ? NN::Convert< N::OSType >( sig ) : N::kUnknownType;
		
		AEEventClass eventClass = NN::Convert< N::FourCharCode >( argEventClass );
		AEEventID    eventID    = NN::Convert< N::FourCharCode >( argEventID    );
		
		using N::kAENoReply;
		using N::kAECanInteract;
		
		N::AESend( BuildAppleEvent( eventClass,
		                            eventID,
		                            SelectAddress( sigCode, app, machine, host, url ),
		                            argBuild.c_str(),
		                            NULL ),
		           kAENoReply | kAECanInteract );
		
		return 0;
	}
	
}

int O::Main( int argc, const char *const argv[] )
{
	return jTools::Main( argc, argv );
}

