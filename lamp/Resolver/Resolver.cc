/*	===========
 *	Resolver.cc
 *	===========
 */

// Standard C
#include "errno.h"

// POSIX
#include "netdb.h"

// Nitrogen
#include "Nitrogen/OpenTransportProviders.h"

// Kerosene
#if TARGET_OS_MAC && !TARGET_RT_MAC_MACHO
#include "SystemCalls.hh"
#endif


	extern "C" { int h_errno; }
	
	namespace N = Nitrogen;
	
	#pragma mark -
	#pragma mark ¥ netdb ¥
	
	struct hostent* gethostbyname( const char* name )
	{
		if ( TARGET_CPU_68K )
		{
			return NULL;
		}
		
	// MWC68K optimizer fails to elide the code below, so we get link errors.
	#if TARGET_CPU_PPC
		
		static ::InetHostInfo  info;
		static struct hostent  host_entry;
		static ::InetHost*     addr_list[10];
		
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
		
	#endif
	}
	
