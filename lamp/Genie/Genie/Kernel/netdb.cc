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
#ifndef __OPENTRANSPORTPROVIDERS__
#include <OpenTransportProviders.h>
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


namespace relix
{
	
	using mac::ot::InternetServices_opened;
	
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
	
	static
	InternetServices_opened InternetServices( netdb_provider_data& data )
	{
		using mac::ot::OpenInternetServices_default_path;
		
		InternetServices_opened opened = OpenInternetServices_default_path();
		
		InetSvcRef p = value( opened );
		
		if ( p )
		{
			OSStatus err;
			
			(err = OTInstallNotifier( p, UPP_ARG( netdb_notifier ), &data ))  ||
			(err = OTSetAsynchronous( p ));
			
			if ( err )
			{
				OTCloseProvider( p );
				
				*(OSStatus*) &opened = err;
			}
		}
		
		return opened;
	}
	
	
	static OSStatus _OTInetStringToAddress( char* name, struct InetHostInfo* result )
	{
		OSStatus err;
		
		OpenTransport_share shared_OpenTransport;
		
		netdb_provider_data data = {};
		
		InternetServices_opened opened = InternetServices( data );
		
		if ( is_error( opened ) )
		{
			return error( opened );
		}
		
		InetSvcRef services = value( opened );
		
		if ( (err = OTInetStringToAddress( services, name, result )) )
		{
			goto bail;
		}
		
		try
		{
			while ( data.code == 0 )
			{
				try_again( false );
			}
			
			err = data.result;
		}
		catch ( ... )
		{
			err = set_errno_from_exception();
		}
		
	bail:
		
		OTCloseProvider( services );
		
		return err;
	}
	
	static OSStatus _OTInetMailExchange( char* domain, UInt16* count, InetMailExchange* result )
	{
		OSStatus err;
		
		OpenTransport_share shared_OpenTransport;
		
		netdb_provider_data data = {};
		
		InternetServices_opened opened = InternetServices( data );
		
		if ( is_error( opened ) )
		{
			return error( opened );
		}
		
		InetSvcRef services = value( opened );
		
		if ( (err = OTInetMailExchange( services, domain, count, result )) )
		{
			goto bail;
		}
		
		try
		{
			while ( data.code == 0 )
			{
				try_again( false );
			}
			
			err = data.result;
		}
		catch ( ... )
		{
			err = set_errno_from_exception();
		}
		
	bail:
		
		OTCloseProvider( services );
		
		return err;
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
