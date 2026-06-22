/*
	location.cc
	-----------
*/

#include "callout/location.hh"

// Mac OS
#ifdef __MACOS__
#ifndef __OSUTILS__
#include <OSUtils.h>
#endif
#endif

// Standard C
#include <string.h>


#if defined(__MACOS__)
#define HAVE_READLOCATION  1
#else
#define HAVE_READLOCATION  0
#endif


namespace v68k    {
namespace callout {

enum
{
	rts = 0x4E75,
};

static
void read_location_Mac( uint8_t* p )
{
#if HAVE_READLOCATION
	
	MachineLocation loc;
	
	ReadLocation( &loc );
	
	Fract lat = loc.latitude;
	Fract lon = loc.longitude;
	Byte  dst = loc.u.dlsDelta;
	long  off = loc.u.gmtDelta;  // only low 24 bits valid
	
	*p++ = lat >> 24;
	*p++ = lat >> 16;
	*p++ = lat >>  8;
	*p++ = lat;
	
	*p++ = lon >> 24;
	*p++ = lon >> 16;
	*p++ = lon >>  8;
	*p++ = lon;
	
	*p++ = dst;
	*p++ = off >> 16;
	*p++ = off >>  8;
	*p++ = off;
	
#endif
}

int32_t ReadLocation_callout( v68k::processor_state& s )
{
	const v68k::function_code_t data_space = s.data_space();
	
	const uint32_t dst = s.a(0);
	
	const uint32_t dst_size = 12;  // 2 4-byte Fracts, 1-byte DLS, 3-byte delta
	
	uint8_t* p;
	
	bool ok = (p = s.translate( dst, dst_size, data_space, mem_write ))  &&
	          ((long) p & 0x1) == 0;
	
	if ( ! ok )
	{
		return (long) p & 0x1 ? v68k::Address_error
		                      : v68k::Bus_error;
	}
	
	if ( ! HAVE_READLOCATION )
	{
		memset( p, '\0', dst_size );
	}
	else
	{
		read_location_Mac( p );
	}
	
	s.d(0) = HAVE_READLOCATION - 1;
	
	s.translate( dst, dst_size, data_space, mem_update );
	
	return rts;
}

}  // namespace callout
}  // namespace v68k
