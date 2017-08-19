/*
	aevt.cc
	-------
*/

// Mac OS X
#ifdef __APPLE__
#include <ApplicationServices/ApplicationServices.h>
#endif

// Mac OS
#ifndef  __AEHELPERS__
#include <AEHelpers.h>
#endif

// Standard C
#include <stdlib.h>
#include <string.h>

// iota
#include "iota/strings.hh"

// AESendBlocking
#include "AESendBlocking.hh"

// command
#include "command/get_option.hh"

// gear
#include "gear/quad.hh"

// plus
#include "plus/string.hh"

// poseven
#include "poseven/functions/write.hh"

// Nitrogen
#include "Nitrogen/AEDataModel.hh"
#include "Nitrogen/AEInteraction.hh"
#include "Nitrogen/Str.hh"

#if CALL_NOT_IN_CARBON

// ClassicToolbox
#include "ClassicToolbox/AppleTalk.hh"
#include "ClassicToolbox/EPPC.hh"
#include "ClassicToolbox/PPCToolbox.hh"

#endif

// Orion
#include "Orion/Main.hh"


using namespace command::constants;

enum
{
	Option_front   = 'F',
	Option_app     = 'a',
	Option_host    = 'h',
	Option_machine = 'm',
	Option_sig     = 's',
	Option_url     = 'u',
	Option_wait    = 'w',
};

static command::option options[] =
{
	{ "front", Option_front },
	{ "wait",  Option_wait  },
	
	{ "app",     Option_app,     Param_required },
	{ "host",    Option_host,    Param_required },
	{ "machine", Option_machine, Param_required },
	{ "sig",     Option_sig,     Param_required },
	{ "signature", Option_sig,   Param_required },
	{ "url",     Option_url,     Param_required },
	{ NULL }
};

static bool front = false;
static bool waits = false;

static const char*  url     = "";
static const char*  host    = NULL;
static const char*  machine = NULL;
static const char*  app     = "";

static const char* sig = "\?\?\?\?";

static char* const* get_options( char* const* argv )
{
	++argv;  // skip arg 0
	
	short opt;
	
	while ( (opt = command::get_option( &argv, options )) )
	{
		switch ( opt )
		{
			case Option_front:
				front = true;
				break;
			
			case Option_wait:
				waits = true;
				break;
			
			case Option_app:
				app = command::global_result.param;
				break;
			
			case Option_host:
				host = command::global_result.param;
				break;
			
			case Option_machine:
				machine = command::global_result.param;
				break;
			
			case Option_sig:
				sig = command::global_result.param;
				break;
			
			case Option_url:
				url = command::global_result.param;
				break;
			
			default:
				abort();
		}
	}
	
	return argv;
}


namespace tool
{
	
	namespace n = nucleus;
	namespace N = Nitrogen;
	namespace p7 = poseven;
	
	
	template < class Quad >
	static inline Quad decode_quad( const char* s )
	{
		return Quad( gear::decode_quad( s ) );
	}
	
	static n::owned< Mac::AppleEvent > BuildAppleEvent( Mac::AEEventClass          eventClass,
	                                                    Mac::AEEventID             eventID,
	                                                    const Mac::AEAddressDesc&  address,
	                                                    const char*                buildString,
	                                                    va_list                    args,
	                                                    Mac::AEReturnID            returnID      = Mac::kAutoGenerateReturnID,
	                                                    Mac::AETransactionID       transactionID = Mac::kAnyTransactionID )
	{
		std::size_t addrSize = N::AEGetDescDataSize( address );
		
		plus::string s;
		
		char* p = s.reset( addrSize );
		
		N::AEGetDescData( address, p, addrSize );
		
		Mac::AppleEvent appleEvent;
		AEBuildError aeErr;
		
		Mac::ThrowOSStatus( ::vAEBuildAppleEvent( eventClass,
		                                          eventID,
		                                          address.descriptorType,
		                                          p,
		                                          addrSize,
		                                          returnID,
		                                          transactionID,
		                                          &appleEvent,
		                                          &aeErr,
		                                          buildString,
		                                          args ) );
		
		return n::owned< Mac::AppleEvent >::seize( appleEvent );
	}
	
#if CALL_NOT_IN_CARBON
	
	static TargetID LocateTarget( const char*  appName,
	                              const char*  machine,
	                              const char*  host )
	{
		PPCPortRec name = n::make< PPCPortRec >( N::Str32( appName ), "\p=" );
		
		LocationNameRec location = machine != NULL ? n::make< LocationNameRec >( n::make< EntityName >( N::Str32( machine ), "\pPPCToolbox" ) )
		                         : host    != NULL ? n::make< LocationNameRec >( n::make< PPCAddrRec >( n::make< PPCXTIAddress >( host ) ) )
		                                           : n::make< LocationNameRec >();
		
		return n::make< TargetID >( N::IPCListPortsSync( name, location ).name, location );
	}
	
#endif
	
	static n::owned< Mac::AEAddressDesc > SelectAddress( bool            front,
	                                                     Mac::FSCreator  sig,
	                                                     const char*     app,
	                                                     const char*     machine,
	                                                     const char*     host,
	                                                     const char*     url )
	{
		if ( front )
		{
			ProcessSerialNumber psn;
			
			Mac::ThrowOSStatus( GetFrontProcess( &psn ) );
			
			return N::AECreateDesc< Mac::typeProcessSerialNumber >( psn );
		}
		else if ( sig != Mac::kUnknownFSCreator )
		{
			return N::AECreateDesc< Mac::typeApplSignature >( sig );
		}
		else
		{
		#if CALL_NOT_IN_CARBON
			
			return N::AECreateDesc< Mac::typeTargetID >( LocateTarget( app, machine, host ) );
			
		#else
			
			return N::AECreateDesc< Mac::typeApplicationURL >( url );
			
		#endif
		}
		
		// Not reached
		return n::owned< Mac::AEAddressDesc >();
	}
	
	static
	n::owned< Mac::AppleEvent > AESendBlocking( const Mac::AppleEvent& event )
	{
		Mac::AppleEvent reply;
		
		Mac::ThrowOSStatus( AESendBlocking( &event, &reply ) );
		
		return n::owned< Mac::AppleEvent >::seize( reply );
	}
	
	static inline
	void AESend( const Mac::AppleEvent& event )
	{
		if ( waits )
		{
			n::owned< Mac::AEDesc_Data > reply = AESendBlocking( event );
			
			n::string output = N::AEGetParamPtr< Mac::typeChar >( reply, Mac::keyDirectObject );
			
			p7::write( p7::stdout_fileno, output.data(), output.size() );
			
			p7::write( p7::stdout_fileno, "\n", 1 );
		}
		else
		{
			N::AESend( event, Mac::kAENoReply | Mac::kAECanInteract );
		}
		
	}
	
	int Main( int argc, char** argv )
	{
		char *const *args = get_options( argv );
		
		const int argn = argc - (args - argv);
		
		char const *const *freeArgs = args;
		
		const char*  argBuild      = NULL;
		const char*  argEventClass = NULL;
		const char*  argEventID    = NULL;
		
		if ( argn == 0 )
		{
			p7::write( p7::stderr_fileno, STR_LEN(
				"Usage:  aevt [-m machine] {-a app | -s sign} class id [params]\n"
				"Examples: aevt -s hhgg aevt quit\n"
				"          aevt -s 'R*ch' misc slct \"'----':obj{want:type(clin), form:indx, seld:42,\n"
				"               from:obj{want:type(cwin), form:indx, seld:1, from:null()}}\"\n" ) );
			
			return 2;
		}
		else if ( argn < 2 )
		{
			p7::write( p7::stderr_fileno, STR_LEN( "aevt: missing arguments" "\n" ) );
			
			return 2;
		}
		else if ( argn < 3 )
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
		
		Mac::FSCreator sigCode = decode_quad< Mac::FSCreator >( sig );
		
		Mac::AEEventClass eventClass = decode_quad< Mac::AEEventClass >( argEventClass );
		Mac::AEEventID    eventID    = decode_quad< Mac::AEEventID    >( argEventID    );
		
		AESend( BuildAppleEvent( eventClass,
		                         eventID,
		                         SelectAddress( front, sigCode, app, machine, host, url ),
		                         argBuild,
		                         NULL ) );
		
		return 0;
	}
	
}
