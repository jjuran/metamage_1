/*
	buffers.cc
	----------
*/

#include "memory/buffers.hh"

// Standard C
#include <string.h>

// v68k-screen
#include "screen/screen.hh"
#include "screen/storage.hh"
#include "screen/surface.hh"

// v68k-memory
#include "memory/memory.hh"


namespace v68k   {
namespace memory {

using v68k::screen::page_1_virtual_buffer;
using v68k::screen::page_2_virtual_buffer;
using v68k::screen::the_surface_shape;
using v68k::screen::virtual_buffer;


int place_buffers( const processor_state& emu, uint8_t* mem, uint32_t mem_top )
{
	enum
	{
		ScreenRow = 0x0106,
		MemTop    = 0x0108,
		SoundBase = 0x0266,
		ScrnBase  = 0x0824,
		CrsrPin   = 0x0834,
	};
	
	if ( mem_top )
	{
		alt_screen_addr  = mem_top - (0x20000 - 0x12700);
		main_screen_addr = mem_top - (0x20000 - 0x1A700);
		main_sound_addr  = mem_top - (0x20000 - 0x1FD00);
		
		emu.put_long( MemTop, mem_top, user_data_space );
	}
	
	if ( ! virtual_buffer )
	{
		page_1_virtual_buffer = mem + main_screen_addr;
		page_2_virtual_buffer = mem + alt_screen_addr;
		
		virtual_buffer = page_1_virtual_buffer;
		
		memset( page_1_virtual_buffer, 0xFF, 64 * 342 );
		memset( page_2_virtual_buffer, 0xFF, 64 * 342 );
	}
	
	const uint32_t screen_row = the_surface_shape.stride;
	const uint32_t screen_res = the_surface_shape.width
	                          | the_surface_shape.height << 16;
	
	emu.put_word( ScreenRow,   screen_row,       user_data_space );
	emu.put_long( SoundBase,   main_sound_addr,  user_data_space );
	emu.put_long( ScrnBase,    main_screen_addr, user_data_space );
	emu.put_long( CrsrPin + 4, screen_res,       user_data_space );
	
	return 0;
}

}  // namespace memory
}  // namespace v68k
