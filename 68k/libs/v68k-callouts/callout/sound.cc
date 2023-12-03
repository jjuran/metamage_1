/*
	sound.cc
	--------
*/

#include "callout/sound.hh"

// Standard C
#include <errno.h>

// v68k-sound
#include "sound/sound.hh"


#ifdef __MWERKS__
#pragma exceptions off
#endif


#ifndef NULL
#define NULL  0L
#endif


namespace v68k    {
namespace callout {

enum
{
	rts = 0x4E75,
};

int32_t send_sound_command_callout( v68k::processor_state& s )
{
	const v68k::function_code_t data_space = s.data_space();
	
	const uint32_t buffer = s.a(0);
	const uint32_t length = s.d(0);
	const uint16_t domain = s.d(1);
	
	const uint8_t* p = s.translate( buffer, length, data_space, mem_read );
	
	if ( p == NULL )
	{
		s.d(0) = -1;
		s.d(1) = EFAULT;
		
		return rts;
	}
	
	long n = v68k::sound::send_command( domain, p, length );
	
	s.d(0) = n;
	
	if ( n < 0 )
	{
		s.d(1) = errno;
	}
	
	return rts;
}

}  // namespace callout
}  // namespace v68k
