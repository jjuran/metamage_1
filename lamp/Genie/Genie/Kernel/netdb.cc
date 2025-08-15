/*	========
 *	netdb.cc
 *	========
 */

// Mac OS X
#ifdef __APPLE__
#include <CoreServices/CoreServices.h>
#endif

// mac-config
#include "mac_config/open-transport.hh"

#if CONFIG_OPEN_TRANSPORT_HEADERS

// Mac OS
#ifndef __OPENTRANSPORT__
#include <OpenTransport.h>
#endif

// POSIX
#include "netdb.h"

// mac-config
#include "mac_config/upp-macros.hh"

// mac-ot-utils
#include "mac_ot/OpenInternetServices.hh"

// mac-app-utils
#include "mac_app/OpenTransport_share.hh"

// relix
#include "relix/api/errno.hh"
#include "relix/api/try_again.hh"
#include "relix/syscall/registry.hh"

// Nitrogen
#include "Nitrogen/OpenTransportProviders.hh"


namespace relix
{
	
	namespace n = nucleus;
	namespace N = Nitrogen;
	
	using mac::app::OpenTransport_share;
	
	
	struct netdb_provider_data
	{
		OTEventCode  code;
		OTResult     result;
	};
	
	static pascal void netdb_notifier( void*        context,
	                                   OTEventCode  code,
	                                   OTResult     result,
	                                   void*        cookie )
	{
		netdb_provider_data* data = (netdb_provider_data*) context;
		
		if ( data == NULL )
		{
			return;
		}
		
		switch ( code )
		{
			case T_DNRSTRINGTOADDRCOMPLETE:
			case T_DNRMAILEXCHANGECOMPLETE:
				data->code   = code;
				data->result = result;
				break;
			
			default:
				break;
		}
	}
	
	
	DEFINE_UPP( OTNotify, netdb_notifier )
	
	static n::owned< InetSvcRef > InternetServices( netdb_provider_data& data )
	{
		using mac::ot::InternetServices_opened;
		using mac::ot::OpenInternetServices_default_path;
		
		InternetServices_opened opened = OpenInternetServices_default_path();
		
		Mac::ThrowOSStatus( error( opened ) );
		
		InetSvcRef p = value( opened );
		
		n::owned< InetSvcRef > provider = n::owned< InetSvcRef >::seize( p );
		
		N::OTInstallNotifier( provider, UPP_ARG( netdb_notifier ), &data );
		
		N::OTSetAsynchronous( provider );
		
		return provider;
	}
	
	
	static OSStatus _OTInetStringToAddress( char* name, struct InetHostInfo* result )
	{
		try
		{
			OpenTransport_share shared_OpenTransport;
			
			netdb_provider_data data = { 0 };
			
			n::owned< InetSvcRef > services = InternetServices( data );
			
			const OSStatus err = ::OTInetStringToAddress( services,
			                                              name,
			                                              result );
			
			if ( err != noErr )
			{
				return err;
			}
			
			while ( data.code == 0 )
			{
				try_again( false );
			}
			
			return data.result;
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
			OpenTransport_share shared_OpenTransport;
			
			netdb_provider_data data = { 0 };
			
			n::owned< InetSvcRef > services = InternetServices( data );
			
			const OSStatus err = ::OTInetMailExchange( services,
			                                           domain,
			                                           count,
			                                           result );
			
			if ( err != noErr )
			{
				return err;
			}
			
			while ( data.code == 0 )
			{
				try_again( false );
			}
			
			return data.result;
		}
		catch ( ... )
		{
			return set_errno_from_exception();
		}
	}
	
#if CONFIG_OPEN_TRANSPORT
	
	#pragma force_active on
	
	REGISTER_SYSTEM_CALL( _OTInetStringToAddress );
	
#if !TARGET_API_MAC_CARBON
	
	REGISTER_SYSTEM_CALL( _OTInetMailExchange );
	
#endif
	
	#pragma force_active reset
	
#endif  // #if CONFIG_OPEN_TRANSPORT
	
}

#endif  // #if CONFIG_OPEN_TRANSPORT_HEADERS
