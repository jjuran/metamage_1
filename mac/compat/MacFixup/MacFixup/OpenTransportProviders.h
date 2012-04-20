/*
	MacFixup/OpenTransportProviders.h
	---------------------------------
	
	Copyright 2010, Joshua Juran
*/

#ifndef MACFIXUP_OPENTRANSPORTPROVIDERS_H
#define MACFIXUP_OPENTRANSPORTPROVIDERS_H

// OpenTransportProviders.h depends on Files.h but doesn't include it
#ifndef __FILES__
#include <Files.h>
#endif

#ifndef __OPENTRANSPORTPROVIDERS__
#ifdef TCP_NODELAY
#error <OpenTransportProviders.h> must be included before <netinet/tcp.h>
#endif
#ifdef IP_OPTIONS
#error <OpenTransportProviders.h> must be included before <netinet/in.h>
#endif
#ifdef AF_ISDN
#error <OpenTransportProviders.h> must be included before <sys/socket.h>
#endif
#include <CIncludes/OpenTransportProviders.h>
#endif

#endif

