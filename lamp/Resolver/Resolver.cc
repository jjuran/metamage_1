/*	===========
 *	Resolver.cc
 *	===========
 */

// Standard C/C++
#include <cstdio>

// Standard C
#include <errno.h>

// POSIX
#include <arpa/inet.h>
#include <netdb.h>

// Nitrogen
#include "Nitrogen/OpenTransportProviders.h"

// Kerosene
#if TARGET_OS_MAC && !TARGET_RT_MAC_MACHO
#include "SystemCalls.hh"
#endif


	namespace N = Nitrogen;
	
	#pragma mark -
	#pragma mark ¥ arpa/inet ¥
	
	int inet_aton( const char* name, struct in_addr* out_addr )
	{
		return -1;
	}
	
	char* inet_ntoa( struct in_addr addr )
	{
		static char result[ 16 ] = { 0 };
		
		std::sprintf( result, "%u.%u.%u.%u", (addr.s_addr >> 24) & 0xFF,
		                                     (addr.s_addr >> 16) & 0xFF,
		                                     (addr.s_addr >>  8) & 0xFF,
		                                     (addr.s_addr      ) & 0xFF );
		
		//::OTInetHostToString( addr.s_addr, result );
		
		return result;
	}
	
	#pragma mark -
	#pragma mark ¥ netdb ¥
	
	extern "C" { int h_errno; }
	
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
	
