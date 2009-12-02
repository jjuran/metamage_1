/*	=======
 *	aevt.cc
 *	=======
 */

// Mac OS
#ifndef  __AEHELPERS__
#include <AEHelpers.h>
#endif

// Standard C++
#include <vector>

// iota
#include "iota/quad.hh"
#include "iota/strings.hh"

// poseven
#include "poseven/functions/write.hh"

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
#include "Orion/get_options.hh"
#include "Orion/Main.hh"


namespace tool
{
	
	namespace N = Nitrogen;
	namespace NN = Nucleus;
	namespace p7 = poseven;
	namespace o = orion;
	
	
	template < class Quad >
	static inline Quad decode_quad( const std::string& s )
	{
		return Quad( iota::decode_quad( s.data() ) );
	}
	
	static NN::Owned< N::AppleEvent > BuildAppleEvent( N::AEEventClass          eventClass,
	                                                   N::AEEventID             eventID,
	                                                   const N::AEAddressDesc&  address,
	                                                   const char*              buildString,
	                                                   va_list                  args,
	                                                   N::AEReturnID            returnID      = N::kAutoGenerateReturnID,
	                                                   N::AETransactionID       transactionID = N::kAnyTransactionID )
	{
		std::size_t addrSize = N::AEGetDescDataSize( address );
		
		std::vector< char > addrData( addrSize );
		
		N::AEGetDescData( address, &addrData.front(), addrSize );
		
		N::AppleEvent appleEvent;
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
		
		return NN::Owned< N::AppleEvent >::Seize( appleEvent );
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
	
	static NN::Owned< N::AEAddressDesc > SelectAddress( N::OSType           sig,
	                                                    const std::string&  app,
	                                                    const std::string&  machine,
	                                                    const std::string&  host,
	                                                    const std::string&  url )
	{
		if ( sig != N::OSType( kUnknownType ) )
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
		return NN::Owned< N::AEAddressDesc >();
	}
	
	// shell$ aevt -m Otter -a Genie |gan Exec '----':[“shutdown”,“-h”]
	// shell$ aevt -s hhgg aevt quit
	
	int Main( int argc, iota::argv_t argv )
	{
		std::string url, host, machine, app, sig;
		
		o::bind_option_to_variable( "-u", url     );
		o::bind_option_to_variable( "-h", host    );
		o::bind_option_to_variable( "-m", machine );
		o::bind_option_to_variable( "-a", app     );
		o::bind_option_to_variable( "-s", sig     );
		
		o::alias_option( "-u", "--url"       );
		o::alias_option( "-h", "--host"      );
		o::alias_option( "-m", "--machine"   );
		o::alias_option( "-a", "--app"       );
		o::alias_option( "-s", "--sig"       );
		o::alias_option( "-s", "--signature" );
		
		o::get_options( argc, argv );
		
		char const *const *freeArgs = o::free_arguments();
		
		std::string argBuild, argEventClass, argEventID;
		
		if ( o::free_argument_count() == 0 )
		{
			p7::write( p7::stderr_fileno, STR_LEN(
				"Usage:  aevt [-m machine] {-a app | -s sign} class id [params]\n"
				"Examples: aevt -s hhgg aevt quit\n"
				"          aevt -m 'Headless Mac' -a Genie |gan Exec \"'----':[“shutdown -h”]\"\n"
				"          aevt -s 'R*ch' misc slct \"'----':obj{want:type(clin), form:indx, seld:42,\n"
				"               from:obj{want:type(cwin), form:indx, seld:1, from:null()}}\"\n" ) );
			
			return 2;
		}
		else if ( o::free_argument_count() < 2 )
		{
			p7::write( p7::stderr_fileno, STR_LEN( "aevt: missing arguments" "\n" ) );
			
			return 2;
		}
		else if ( o::free_argument_count() < 3 )
		{
			argBuild = "";
		}
		else
		{
			argBuild = freeArgs[2];
		}
		
		argEventClass = freeArgs[0];
		argEventID    = freeArgs[1];
		
		if ( argEventClass.size() != 4  ||  argEventID.size() != 4 )
		{
			p7::write( p7::stderr_fileno, STR_LEN( "aevt: invalid parameter" "\n" ) );
			
			return 1;
		}
		
		N::OSType sigCode = (sig.size() == 4) ? decode_quad< N::OSType >( sig )
		                                      : N::kUnknownType;
		
		N::AEEventClass eventClass = decode_quad< N::AEEventClass >( argEventClass );
		N::AEEventID    eventID    = decode_quad< N::AEEventID    >( argEventID    );
		
		N::AESend( BuildAppleEvent( eventClass,
		                            eventID,
		                            SelectAddress( sigCode, app, machine, host, url ),
		                            argBuild.c_str(),
		                            NULL ),
		           N::kAENoReply | N::kAECanInteract );
		
		return 0;
	}
	
}

