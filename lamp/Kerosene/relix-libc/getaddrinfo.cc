/*
	getaddrinfo.cc
	--------------
*/

// Mac OS
#ifndef __FILES__
#include <Files.h>
#endif
#ifndef __OPENTRANSPORTPROVIDERS__
#include <OpenTransportProviders.h>
#endif

// POSIX
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>

// Standard C
#include <stdlib.h>

// gear
#include "gear/parse_decimal.hh"

// Relix
#include "relix/_OTInetStringToAddress.h"


#pragma exceptions off


struct combined_addrinfo_sockaddr
{
	addrinfo     info;
	sockaddr_in  addr;
};

typedef combined_addrinfo_sockaddr gai_record;

static
void fill_info( gai_record* record, int type, int protocol, int port, int addr )
{
	record->info.ai_family   = PF_INET;
	record->info.ai_socktype = type;
	record->info.ai_protocol = protocol;
	record->info.ai_addrlen  = sizeof (sockaddr_in);
	record->info.ai_addr     = (sockaddr*) &record->addr;
	
	record->addr.sin_family = AF_INET;
	record->addr.sin_port   = port;
	record->addr.sin_addr.s_addr = addr;
}

int getaddrinfo( const char*      nodename,
                 const char*      service,
                 const addrinfo*  hints,
                 addrinfo**       result )
{
	if ( nodename == NULL  &&  service == NULL )
	{
		return EAI_NONAME;
	}
	
	bool tcp_stream = true;
	bool udp_dgram  = true;
	
	if ( hints )
	{
		const int allowed_flags = AI_NUMERICSERV | AI_PASSIVE;
		
		if ( hints->ai_flags & ~allowed_flags )
		{
			return EAI_BADFLAGS;
		}
		
		if ( nodename == NULL  &&  hints->ai_flags & AI_PASSIVE )
		{
			nodename = "0.0.0.0";
		}
		
		const int family = hints->ai_family;
		
		if ( family != PF_UNSPEC  &&  family != PF_INET )
		{
			return EAI_FAMILY;
		}
		
		switch ( hints->ai_socktype )
		{
			case 0:
				break;
			
			case SOCK_STREAM:  udp_dgram = false;  break;
			case SOCK_DGRAM:  tcp_stream = false;  break;
			
			default:
				return EAI_SOCKTYPE;
		}
		
		switch ( hints->ai_protocol )
		{
			case 0:
				break;
			
			case IPPROTO_TCP:  udp_dgram  = false;  break;
			case IPPROTO_UDP:  tcp_stream = false;  break;
			
			default:
				return EAI_PROTOCOL;
		}
	}
	
	int factor = tcp_stream + udp_dgram;
	
	if ( factor == 0 )
	{
		return EAI_BADHINTS;
	}
	
	if ( nodename == NULL )
	{
		nodename = "127.0.0.1";
	}
	
	if ( service == NULL )
	{
		service = "0";
	}
	
	::InetHostInfo hostinfo;
	
	const OSStatus err = _OTInetStringToAddress( (char*) nodename, &hostinfo );
	
	if ( err != noErr )
	{
		switch ( err )
		{
			case kOTBadNameErr:  return EAI_NONAME;
			case kOTNoDataErr:   return EAI_AGAIN;
			default:             return EAI_FAIL;
		}
	}
	
	unsigned port = gear::parse_unsigned_decimal( service );
	unsigned addr = hostinfo.addrs[ 0 ];
	
	gai_record* record = (gai_record*) calloc( factor, sizeof (gai_record) );
	
	if ( record == NULL )
	{
		return EAI_MEMORY;
	}
	
	*result = &record->info;
	
	if ( tcp_stream )
	{
		fill_info( record, SOCK_STREAM, IPPROTO_TCP, port, addr );
		
		if ( udp_dgram )
		{
			gai_record* prev = record++;
			
			prev->info.ai_next = &record->info;
		}
	}
	
	if ( udp_dgram )
	{
		fill_info( record, SOCK_DGRAM, IPPROTO_UDP, port, addr );
	}
	
	return 0;
}

void freeaddrinfo( addrinfo* ai )
{
	free( ai );
}
