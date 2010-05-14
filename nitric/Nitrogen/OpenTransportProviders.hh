// Nitrogen/OpenTransportProviders.hh
// ----------------------------------
//
// Maintained by Joshua Juran

// Part of the Nitrogen project.
//
// Written 2005-2009 by Joshua Juran.
//
// This code was written entirely by the above contributor, who places it
// in the public domain.


#ifndef NITROGEN_OPENTRANSPORTPROVIDERS_HH
#define NITROGEN_OPENTRANSPORTPROVIDERS_HH

// Mac OS
// OpenTransportProviders.h depends on FSSpec but doesn't include Files.h.
#ifndef __FILES__
#include <Files.h>
#endif
#ifndef __OPENTRANSPORTPROVIDERS__
#include <OpenTransportProviders.h>
#endif

// Standard C++
#include <vector>

// iota
#include "iota/string_traits.hh"

// nucleus
#include "nucleus/enumeration_traits.hh"
#include "nucleus/initialize.hh"
#include "nucleus/string.hh"

// Nitrogen
#ifndef NITROGEN_OPENTRANSPORT_HH
#include "Nitrogen/OpenTransport.hh"
#endif


namespace Nitrogen
{
	
	class DefaultInternetServicesPath {};
	
}

#ifdef kDefaultInternetServicesPath
#undef kDefaultInternetServicesPath

// This definition is useless to clients because OTOpenInternetServices[InContext]() takes
// nucleus::owned< OTConfigurationRef >, not OTConfigurationRef.  You can't get there from here.
// Instead, we define a new class and overload on that.

//static const OTConfigurationRef kDefaultInternetServicesPath = (OTConfigurationRef)-3L;

extern const Nitrogen::DefaultInternetServicesPath kDefaultInternetServicesPath;

#endif

// However, we still need to use it internally, so we define it in OpenTransportProviders.cc.


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
	
	enum InetPort
	{
		kInetPort_Max = nucleus::enumeration_traits< ::InetPort >::max
	};
	
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
	
}

namespace nucleus
{
	
	template <> struct disposer_traits< Nitrogen::InetSvcRef > : disposer_traits< Nitrogen::ProviderRef > {};
	
}

namespace Nitrogen
{
	
	using ::InetAddress;
	
	typedef nucleus::string InetDomainName;
	
	using ::InetHostInfo;
	using ::InetSysInfo;
	using ::InetMailExchange;
	
	InetAddress& OTInitInetAddress( InetAddress& addr, InetPort port, InetHost host );
	
}

namespace nucleus
{
	
	template <>
	struct initializer< Nitrogen::InetAddress >
	{
		Nitrogen::InetAddress& operator()( Nitrogen::InetAddress&  addr,
		                                   Nitrogen::InetPort      port,
		                                   Nitrogen::InetHost      host )
		{
			return Nitrogen::OTInitInetAddress( addr, port, host );
		}
	};
	
}

namespace Nitrogen
{
	
	#pragma mark -
	#pragma mark ¥ IP address encoding ¥
	
	InetHost OTInetStringToHost( const char* str );
	
	template < class String >
	inline InetHost OTInetStringToHost( const String& str )
	{
		return OTInetStringToHost( iota::get_string_c_str( str ) );
	}
	
	nucleus::string OTInetHostToString( InetHost host );
	
}

namespace Nitrogen
{
	
	#pragma mark -
	#pragma mark ¥ DNR ¥
	
	nucleus::owned< InetSvcRef > OTOpenInternetServicesInContext( nucleus::owned< OTConfigurationRef >  cfig,
	                                                              OTClientContextPtr                    clientContext = NULL );
	
	nucleus::owned< InetSvcRef > OTOpenInternetServicesInContext( DefaultInternetServicesPath  /**/,
	                                                              OTClientContextPtr           clientContext = NULL );
	
	InetHostInfo& OTInetStringToAddress( InetSvcRef     ref,
	                                     char*          name,
	                                     InetHostInfo&  hInfo );
	
	inline InetHostInfo OTInetStringToAddress( InetSvcRef ref, char* name )
	{
		InetHostInfo result;
		
		return OTInetStringToAddress( ref, name, result );
	}
	
	InetDomainName OTInetAddressToName( InetSvcRef ref, InetHost addr );
	
	// OTInetSysInfo
	
	void OTInetMailExchange( InetSvcRef                        ref,
	                         char*                             name,
	                         std::vector< InetMailExchange >&  mx );
	
}

#endif

