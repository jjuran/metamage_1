/*
	screen.cc
	---------
*/

#include "screen.hh"

// POSIX
#include <errno.h>
#include <sys/mman.h>

// xv68k
#include "shared_memory.hh"


static void* the_screen_buffer;


int set_screen_backing_store_file( const char* path )
{
	the_screen_buffer = open_shared_memory( path, screen_size );
	
	if ( the_screen_buffer == 0 )  // NULL
	{
		return errno;
	}
	
	return 0;
}

uint8_t* screen_memory::translate( uint32_t               addr,
                                   uint32_t               length,
                                   v68k::function_code_t  fc,
                                   v68k::memory_access_t  access ) const
{
	if ( access == v68k::mem_exec )
	{
		//return 0;  // NULL
	}
	
	if ( the_screen_buffer == 0 )  // NULL
	{
		return 0;  // NULL
	}
	
	try
	{
		if ( length > screen_size )
		{
			// The memory access is somehow wider than the buffer is long
			return 0;  // NULL
		}
		
		if ( addr > screen_size - length )
		{
			return 0;  // NULL
		}
		
		uint8_t* p = (uint8_t*) the_screen_buffer + addr;
		
		if ( access == v68k::mem_update )
		{
			msync( the_screen_buffer, screen_size, MS_SYNC );
		}
		
		return p;
	}
	catch ( ... )
	{
		return 0;  // NULL
	}
}
