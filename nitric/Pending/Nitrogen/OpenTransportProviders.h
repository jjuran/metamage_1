// ========================
// OpenTransportProviders.h
// ========================

#ifndef NITROGEN_OPENTRANSPORTPROVIDERS_H
#define NITROGEN_OPENTRANSPORTPROVIDERS_H

// Universal Interfaces
#ifndef __OPENTRANSPORTPROVIDERS__
#include <OpenTransportProviders.h>
#endif

// Standard C++
#include <vector>

// Nitrogen core
#ifndef NITROGEN_IDTYPE_H
#include "Nitrogen/IDType.h"
#endif
#ifndef NITROGEN_INITIALIZE_H
#include "Nitrogen/Initialize.h"
#endif
#ifndef NITROGEN_SELECTORTYPE_H
#include "Nitrogen/SelectorType.h"
#endif

// Nitrogen / Carbon support
#ifndef NITROGEN_OPENTRANSPORT_H
#include "Nitrogen/OpenTransport.h"
#endif

namespace Nitrogen
{
	
	class DefaultInternetServicesPath {};
	
}

#ifdef kDefaultInternetServicesPath
#undef kDefaultInternetServicesPath

// This definition is useless to clients because OTOpenInternetServices[InContext]() takes
// Owned< OTConfigurationRef >, not OTConfigurationRef.  You can't get there from here.
// Instead, we define a new class and overload on that.

//static const OTConfigurationRef kDefaultInternetServicesPath = (OTConfigurationRef)-3L;

static const Nitrogen::DefaultInternetServicesPath
       kDefaultInternetServicesPath = Nitrogen::DefaultInternetServicesPath();

#endif

// However, we still need to use it internally, so we define it under a different name.

namespace Nitrogen
{
	
	static const OTConfigurationRef
	       kDefaultInternetServicesPath_Internal = (OTConfigurationRef)-3L;
	
}


#ifdef SET_TOS
#undef SET_TOS

inline unsigned char SET_TOS( unsigned char prec, unsigned char tos )
{
	return (0x7 & prec) << 5  |  (0x1c & tos);
}

#endif

#ifdef OTOpenInternetServices
#undef OTOpenInternetServices
#endif


namespace Nitrogen
{
	
	class InetPort_Tag {};
	typedef IDType< InetPort_Tag, ::InetPort, 0 > InetPort;
	
	/*
	class InetHost_Tag {};
	typedef IDType< InetHost_Tag, ::InetHost, 0 > InetHost;
	*/
	
	// Default InetHost is kOTAnyInetAddress (i.e. 0).
	// Alternative choice is 0xFFFFFFFF (no address).
	
	class InetHost
	{
		private:
			union
			{
				::InetHost address;
				unsigned char octets[ 4 ];  // This makes debugging easier
			};
		
		public:
			InetHost() : address( kOTAnyInetAddress )  {}
			
			InetHost( ::InetHost addr ) : address( addr )  {}
			
			InetHost( unsigned char a,
			          unsigned char b,
			          unsigned char c,
			          unsigned char d ) : address( a << 24 | b << 16 | c << 8 | d )  {}
			
			static InetHost Make( ::InetHost addr )  { return InetHost( addr ); }
			
			::InetHost Get() const  { return address; }
			operator ::InetHost() const  { return Get(); }
	};
	
	using ::InetSvcRef;
	
	template <>
	struct OwnedDefaults< InetSvcRef > : OwnedDefaults< ProviderRef > {};
	
	using ::InetAddress;
	
	typedef std::string InetDomainName;
	
	using ::InetHostInfo;
	using ::InetSysInfo;
	using ::InetMailExchange;
	
	InetAddress& OTInitInetAddress( InetAddress& addr, InetPort port, InetHost host );
	
	template <>
	struct Initializer< InetAddress >
	{
		InetAddress& operator()( InetAddress& addr, InetPort port, InetHost host )
		{
			return OTInitInetAddress( addr, port, host );
		}
	};
	
	#pragma mark -
	#pragma mark ¥ IP address encoding ¥
	
	InetHost OTInetStringToHost( const char* str );
	
	inline InetHost OTInetStringToHost( const std::string& str )
	{
		return OTInetStringToHost( str.c_str() );
	}
	
	std::string OTInetHostToString( InetHost host );
	
	template <>
	struct Converter< InetHost, const char* > : std::unary_function< const char*, InetHost >
	{
		InetHost operator()( const char* input ) const
		{
			return OTInetStringToHost( input );
		}
	};
	
	template <>
	struct Converter< InetHost, std::string > : std::unary_function< std::string, InetHost >
	{
		InetHost operator()( const std::string& input ) const
		{
			return OTInetStringToHost( input );
		}
	};
	
	template <>
	struct Converter< std::string, InetHost > : std::unary_function< InetHost, std::string >
	{
		std::string operator()( InetHost input ) const
		{
			return OTInetHostToString( input );
		}
	};
	
	#pragma mark -
	#pragma mark ¥ DNR ¥
	
	Owned< InetSvcRef > OTOpenInternetServicesInContext( Owned< OTConfigurationRef >  cfig,
	                                                     OTClientContextPtr           clientContext = NULL );
	
	Owned< InetSvcRef > OTOpenInternetServicesInContext( DefaultInternetServicesPath,
	                                                     OTClientContextPtr           clientContext = NULL );
	
	inline Owned< InetSvcRef > OTOpenInternetServices( Owned< OTConfigurationRef > cfig )
	{
		return OTOpenInternetServicesInContext( cfig );
	}
	
	inline Owned< InetSvcRef > OTOpenInternetServices( DefaultInternetServicesPath )
	{
		return OTOpenInternetServicesInContext( kDefaultInternetServicesPath );
	}
	
	InetHostInfo& OTInetStringToAddress( InetSvcRef ref, char* name, InetHostInfo& hInfo );
	
	inline InetHostInfo OTInetStringToAddress( InetSvcRef ref, char* name )
	{
		InetHostInfo result;
		
		return OTInetStringToAddress( ref, name, result );
	}
	
	InetDomainName OTInetAddressToName( InetSvcRef ref, InetHost addr );
	
	// OTInetSysInfo
	
	void OTInetMailExchange( InetSvcRef ref,
	                         char* name,
	                         std::vector< InetMailExchange >& mx );
	
}

#endif

