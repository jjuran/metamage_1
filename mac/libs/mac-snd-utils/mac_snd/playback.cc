/*
	playback.cc
	-----------
*/

#include "mac_snd/playback.hh"

// Mac OS X
#ifdef __APPLE__
#include <CoreServices/CoreServices.h>
#endif

// Mac OS
#ifndef MAC_OS_X_VERSION_10_5
#ifndef __DEVICES__
#include <Devices.h>
#endif
#endif


namespace mac {
namespace snd {

short play_sound( const void* buffer, long size )
{
	return FSWrite( -4, &size, buffer );
}

}
}
