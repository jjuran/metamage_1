/*
	gethostbyname.cc
	----------------
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

// Relix
#include "relix/_OTInetStringToAddress.h"


struct hostent* gethostbyname( const char* name )
{
	static ::InetHostInfo  hostinfo;
	static struct hostent  host_entry;
	static ::InetHost*     addr_list[10];
	
	const OSStatus err = _OTInetStringToAddress( (char*) name, &hostinfo );
	
	if ( err != noErr )
	{
		switch ( err )
		{
			case kOTBadNameErr:  h_errno = HOST_NOT_FOUND;  break;
			case kOTNoDataErr:   h_errno = TRY_AGAIN;       break;
			
			default:
				h_errno = NO_RECOVERY;  // ???
				break;
		}
		
		return NULL;
	}
	
	addr_list[0] = &hostinfo.addrs[0];
	
	host_entry.h_name      = hostinfo.name;
	host_entry.h_aliases   = NULL;
	host_entry.h_addrtype  = AF_INET;
	host_entry.h_length    = sizeof (::InetHost);
	host_entry.h_addr_list = reinterpret_cast< char** >( addr_list );
	
	return &host_entry;
}
