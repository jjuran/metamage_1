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

// Standard C
#include <string.h>

// iota
#include "iota/quad.hh"
#include "iota/strings.hh"

// poseven
#include "poseven/functions/write.hh"

// Nitrogen
#include "Nitrogen/AEInteraction.hh"
#include "Nitrogen/Processes.hh"
#include "Nitrogen/Str.hh"

#if CALL_NOT_IN_CARBON

// ClassicToolbox
#include "ClassicToolbox/AppleTalk.hh"
#include "ClassicToolbox/EPPC.hh"
#include "ClassicToolbox/PPCToolbox.hh"

#endif

// Orion
#include "Orion/get_options.hh"
#include "Orion/Main.hh"


namespace tool
{
	
	namespace n = nucleus;
	namespace N = Nitrogen;
	namespace p7 = poseven;
	namespace o = orion;
	
	
	template < class Quad >
	static inline Quad decode_quad( const char* s )
	{
		return Quad( iota::decode_quad( s ) );
	}
	
	static n::owned< N::AppleEvent > BuildAppleEvent( N::AEEventClass          eventClass,
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
		
		return n::owned< N::AppleEvent >::seize( appleEvent );
	}
	
#if CALL_NOT_IN_CARBON
	
	static TargetID LocateTarget( const char*  appName,
	                              const char*  machine,
	                              const char*  host )
	{
		PPCPortRec name = n::make< PPCPortRec >( N::Str32( appName ), "\p=" );
		
		/*
		LocationNameRec location = machine.empty() ? host.empty() ? n::make< LocationNameRec >()
		                                                          : n::make< LocationNameRec >( n::make< PPCAddrRec >( n::make< PPCXTIAddress >( host ) ) )
		                                           : n::make< LocationNameRec >( n::make< EntityName >( N::Str32( machine ), "\pPPCToolbox" ) );
		*/
		
		LocationNameRec location = machine != NULL ? n::make< LocationNameRec >( n::make< EntityName >( N::Str32( machine ), "\pPPCToolbox" ) )
		                         : host    != NULL ? n::make< LocationNameRec >( n::make< PPCAddrRec >( n::make< PPCXTIAddress >( host ) ) )
		                                           : n::make< LocationNameRec >();
		
		return n::make< TargetID >( N::IPCListPortsSync( name, location ).name, location );
	}
	
#endif
	
	static n::owned< N::AEAddressDesc > SelectAddress( bool         front,
	                                                   N::OSType    sig,
	                                                   const char*  app,
	                                                   const char*  machine,
	                                                   const char*  host,
	                                                   const char*  url )
	{
		if ( front )
		{
			return N::AECreateDesc< N::typeProcessSerialNumber >( N::GetFrontProcess() );
		}
		else if ( sig != N::OSType( kUnknownType ) )
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
		return n::owned< N::AEAddressDesc >();
	}
	
	// shell$ aevt -m Otter -a Genie |gan Exec '----':[“shutdown”,“-h”]
	// shell$ aevt -s hhgg aevt quit
	
	int Main( int argc, char** argv )
	{
		bool front = false;
		
		const char*  url     = "";
		const char*  host    = NULL;
		const char*  machine = NULL;
		const char*  app     = "";
		
		const char* sig = "\?\?\?\?";
		
		o::bind_option_to_variable( "-F", front   );
		o::bind_option_to_variable( "-u", url     );
		o::bind_option_to_variable( "-h", host    );
		o::bind_option_to_variable( "-m", machine );
		o::bind_option_to_variable( "-a", app     );
		o::bind_option_to_variable( "-s", sig     );
		
		o::alias_option( "-F", "--front"     );
		o::alias_option( "-u", "--url"       );
		o::alias_option( "-h", "--host"      );
		o::alias_option( "-m", "--machine"   );
		o::alias_option( "-a", "--app"       );
		o::alias_option( "-s", "--sig"       );
		o::alias_option( "-s", "--signature" );
		
		o::get_options( argc, argv );
		
		char const *const *freeArgs = o::free_arguments();
		
		const char*  argBuild      = NULL;
		const char*  argEventClass = NULL;
		const char*  argEventID    = NULL;
		
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
		
		if ( strlen( argEventClass ) != 4  ||  strlen( argEventID ) != 4 )
		{
			p7::write( p7::stderr_fileno, STR_LEN( "aevt: invalid parameter" "\n" ) );
			
			return 1;
		}
		
		N::OSType sigCode = decode_quad< N::OSType >( sig );
		
		N::AEEventClass eventClass = decode_quad< N::AEEventClass >( argEventClass );
		N::AEEventID    eventID    = decode_quad< N::AEEventID    >( argEventID    );
		
		N::AESend( BuildAppleEvent( eventClass,
		                            eventID,
		                            SelectAddress( front, sigCode, app, machine, host, url ),
		                            argBuild,
		                            NULL ),
		           N::kAENoReply | N::kAECanInteract );
		
		return 0;
	}
	
}

