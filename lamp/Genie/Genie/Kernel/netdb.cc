/*	========
 *	netdb.cc
 *	========
 */

// Mac OS
#ifndef __OPENTRANSPORT__
#include <OpenTransport.h>
#endif

// POSIX
#include "netdb.h"

// ClassicToolbox
#include "ClassicToolbox/OpenTransportProviders.hh"

// Genie
#include "Genie/current_process.hh"
#include "Genie/Process.hh"
#include "Genie/SystemCallRegistry.hh"
#include "Genie/Utilities/ShareOpenTransport.hh"


namespace Genie
{
	
	namespace n = nucleus;
	namespace N = Nitrogen;
	
	
	static pascal void YieldingNotifier( void*        context,
	                                     OTEventCode  code,
	                                     OTResult     result,
	                                     void*        cookie )
	{
		switch ( code )
		{
			case kOTSyncIdleEvent:
				try
				{
					Yield( kInterruptNever );  // FIXME
				}
				catch ( ... )
				{
				}
				
				break;
			
			case kOTProviderWillClose:
				break;
			
			case kOTProviderIsClosed:
				break;
			
			default:
				break;
		}
	}
	
	
	static n::owned< InetSvcRef > InternetServices()
	{
		static OTNotifyUPP gNotifyUPP = ::NewOTNotifyUPP( YieldingNotifier );
		
		n::owned< N::InetSvcRef > provider = N::OTOpenInternetServices( kDefaultInternetServicesPath );
		
		N::OTInstallNotifier( provider, gNotifyUPP, NULL );
		
		N::OTUseSyncIdleEvents( provider, true );
		
		return provider;
	}
	
	
	static OSStatus _OTInetStringToAddress( char* name, struct InetHostInfo* result )
	{
		try
		{
			OpenTransportShare sharedOpenTransport;
			
			return ::OTInetStringToAddress( InternetServices(),
			                                name,
			                                result );
		}
		catch ( ... )
		{
			return set_errno_from_exception();
		}
	}
	
	static OSStatus _OTInetMailExchange( char* domain, UInt16* count, InetMailExchange* result )
	{
		try
		{
			OpenTransportShare sharedOpenTransport;
			
			return ::OTInetMailExchange( InternetServices(),
			                             domain,
			                             count,
			                             result );
		}
		catch ( ... )
		{
			return set_errno_from_exception();
		}
	}
	
	#pragma force_active on
	
	REGISTER_SYSTEM_CALL( _OTInetStringToAddress );
	
#if !TARGET_API_MAC_CARBON
	
	REGISTER_SYSTEM_CALL( _OTInetMailExchange );
	
#endif
	
	#pragma force_active reset
	
}

