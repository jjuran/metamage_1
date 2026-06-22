/*
	location.cc
	-----------
*/

#include "callout/location.hh"

// Standard C
#include <string.h>


#define HAVE_READLOCATION  0


namespace v68k    {
namespace callout {

enum
{
	rts = 0x4E75,
};

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
	
	s.d(0) = HAVE_READLOCATION - 1;
	
	s.translate( dst, dst_size, data_space, mem_update );
	
	return rts;
}

}  // namespace callout
}  // namespace v68k
