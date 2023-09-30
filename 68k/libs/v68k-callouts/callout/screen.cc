/*
	screen.cc
	---------
*/

#include "callout/screen.hh"

// v68k-screen
#include "screen/lock.hh"
#include "screen/surface.hh"
#include "screen/update.hh"


#ifdef __MWERKS__
#pragma exceptions off
#endif


#ifndef NULL
#define NULL  0L
#endif


namespace v68k    {
namespace callout {

using v68k::screen::lock_level;


enum
{
	nop = 0x4E71,
	rts = 0x4E75,
};

int32_t ScrnBitMap_callout( v68k::processor_state& s )
{
	const v68k::function_code_t data_space = s.data_space();
	
	uint32_t& sp = s.a(7);
	
	uint32_t return_address;
	uint32_t pointer;
	
	if ( ! s.get_long( sp, return_address, data_space ) )
	{
		return v68k::Bus_error;
	}
	
	sp += 4;
	
	if ( ! s.get_long( sp, pointer, data_space ) )
	{
		return v68k::Bus_error;
	}
	
	sp += 4;
	
	const uint32_t n = sizeof (uint32_t)
	                 + sizeof (uint16_t)
	                 + sizeof (uint16_t) * 4;
	
	uint8_t* p = s.translate( pointer, n, data_space, mem_write );
	
	if ( p == NULL )
	{
		return v68k::Bus_error;
	}
	
	using v68k::screen::the_surface_shape;
	
	const uint32_t baseAddr = 0x0001A700;
	const uint16_t rowBytes = the_surface_shape.stride;
	const uint16_t height   = the_surface_shape.height;
	const uint16_t width    = the_surface_shape.width;
	
	*p++ = uint8_t( baseAddr >> 24 );
	*p++ = uint8_t( baseAddr >> 16 );
	*p++ = uint8_t( baseAddr >>  8 );
	*p++ = uint8_t( baseAddr       );
	
	*p++ = uint8_t( rowBytes >> 8 );
	*p++ = uint8_t( rowBytes      );
	
	*p++ = 0;
	*p++ = 0;
	
	*p++ = 0;
	*p++ = 0;
	
	*p++ = uint8_t( height >> 8 );
	*p++ = uint8_t( height      );
	
	*p++ = uint8_t( width >> 8 );
	*p++ = uint8_t( width      );
	
	s.pc() = return_address - 2;
	
	return nop;
}

int32_t lock_screen_callout( v68k::processor_state& s )
{
	--lock_level;
	
	return rts;
}

int32_t unlock_screen_callout( v68k::processor_state& s )
{
	if ( ++lock_level == 0 )
	{
		v68k::screen::update();
	}
	
	return rts;
}

}  // namespace callout
}  // namespace v68k
