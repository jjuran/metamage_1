/*
	screen.cc
	---------
*/

#include "memory/screen.hh"

// Standard C
#include <stdlib.h>
#include <string.h>

// rasterlib
#include "raster/clut_detail.hh"

// v68k-screen
#include "screen/screen.hh"
#include "screen/storage.hh"

// v68k-memory
#include "memory/memory.hh"


namespace v68k   {
namespace memory {

using raster::clut_data;
using raster::clut_header;

using v68k::screen::page_1_virtual_buffer;
using v68k::screen::the_screen_size;
using v68k::screen::transit_clut;
using v68k::screen::virtual_buffer;
using v68k::screen::virtual_clut;

static clut_header null_clut;

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
		
		virtual_clut = (clut_data*) &null_clut;
		
		if ( transit_clut )
		{
			size_t size = sizeof_clut( *transit_clut );
			
			virtual_clut = (clut_data*) calloc( size, 1 );
			
			if ( ! virtual_clut )
			{
				// We leak virtual_buffer, but caller will exit() anyway.
				
				return -1;
			}
			
			virtual_clut->max = transit_clut->max;
		}
	}
	
	return 0;
}

}  // namespace memory
}  // namespace v68k
