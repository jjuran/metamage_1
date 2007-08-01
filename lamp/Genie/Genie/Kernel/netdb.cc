/*	========
 *	netdb.cc
 *	========
 */

// POSIX
#include "netdb.h"

// Nitrogen
#include "Nitrogen/OpenTransportProviders.h"

// Genie
#include "Genie/SystemCallRegistry.hh"
#include "Genie/SystemCalls.hh"
#include "Genie/Utilities/ShareOpenTransport.hh"


namespace Genie
{
	
	DECLARE_MODULE_INIT( Kernel_netdb )
	DEFINE_MODULE_INIT(  Kernel_netdb )
	
	namespace N = Nitrogen;
	namespace NN = Nucleus;
	
	
	// Special guest appearance
	static NN::Owned< InetSvcRef > InternetServices()
	{
		//static 
		       NN::Owned< InetSvcRef >
		       gInetSvcRef = N::OTOpenInternetServices( kDefaultInternetServicesPath );
		
		return gInetSvcRef;
	}
	
	
	static struct hostent* gethostbyname( const char* name )
	{
		SystemCallFrame frame( "gethostbyname" );
		
		static ::InetHostInfo  info;
		static struct hostent  host_entry;
		static ::InetHost*     addr_list[10];
		
		OpenTransportShare sharedOpenTransport;
		
		try
		{
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
	
	REGISTER_SYSTEM_CALL( gethostbyname );
	
	static OSStatus OTInetMailExchange( char* domain, UInt16* count, InetMailExchange* result )
	{
		SystemCallFrame frame( "OTInetMailExchange" );
		
		OpenTransportShare sharedOpenTransport;
		
		return ::OTInetMailExchange( InternetServices(), domain, count, result );
	}
	
	REGISTER_SYSTEM_CALL( OTInetMailExchange );
	
}

