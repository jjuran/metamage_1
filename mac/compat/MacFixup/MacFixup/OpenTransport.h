/*
	MacFixup/OpenTransport.h
	------------------------
*/

#ifndef MACFIXUP_OPENTRANSPORT_H
#define MACFIXUP_OPENTRANSPORT_H

#ifndef __OPENTRANSPORT__
#if !TARGET_API_MAC_CARBON  &&  defined( O_ASYNC )
#error <OpenTransport.h> must be included before <fcntl.h> for non-Carbon
#endif
#if (OTKERNEL || OTUNIXERRORS)  &&  defined( EDEADLK )
#error <OpenTransport.h> must be included before <sys/errno.h> if OTKERNEL or OTUNIXERRORS
#endif
#ifdef SIGHUP
#error <OpenTransport.h> must be included before <sys/signal.h>
#endif
#include <CIncludes/OpenTransport.h>
#endif

#endif

