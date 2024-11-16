/*
	screen.cc
	---------
*/

#include "memory/screen.hh"

// Standard C
#include <stdlib.h>
#include <string.h>

// v68k-screen
#include "screen/screen.hh"
#include "screen/storage.hh"

// v68k-memory
#include "memory/memory.hh"


namespace v68k   {
namespace memory {

using v68k::screen::page_1_virtual_buffer;
using v68k::screen::the_screen_size;
using v68k::screen::virtual_buffer;


int allocate_screen()
{
	if ( the_screen_size != 21888 )
	{
		virtual_buffer = malloc( the_screen_size );
		
		if ( ! virtual_buffer )
		{
			return -1;
		}
		
		memset( virtual_buffer, 0xFF, the_screen_size );
		
		page_1_virtual_buffer = (uint8_t*) virtual_buffer;
		
		main_screen_addr = 0x00E00000;
	}
	
	return 0;
}

}  // namespace memory
}  // namespace v68k
