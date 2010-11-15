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
#include "Genie/SystemCalls.hh"
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
	
	
	static struct hostent* gethostbyname( const char* name )
	{
		SystemCallFrame frame( "gethostbyname" );
		
		static ::InetHostInfo  info;
		static struct hostent  host_entry;
		static ::InetHost*     addr_list[10];
		
		try
		{
			OpenTransportShare sharedOpenTransport;
			
			info = N::OTInetStringToAddress( InternetServices(),
			                                 (char*) name );
		}
		catch ( ... )
		{
			return NULL;
		}
		
		addr_list[0] = &info.addrs[0];
		
		host_entry.h_name      = info.name;
		host_entry.h_aliases   = NULL;
		host_entry.h_addrtype  = AF_INET;
		host_entry.h_length    = sizeof (::InetHost);
		host_entry.h_addr_list = reinterpret_cast< char** >( addr_list );
		
		return &host_entry;
	}
	
	static OSStatus _OTInetMailExchange( char* domain, UInt16* count, InetMailExchange* result )
	{
		SystemCallFrame frame( "_OTInetMailExchange" );
		
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
	
	REGISTER_SYSTEM_CALL( gethostbyname );
	
#if !TARGET_API_MAC_CARBON
	
	REGISTER_SYSTEM_CALL( _OTInetMailExchange );
	
#endif
	
	#pragma force_active reset
	
}

