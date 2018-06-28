/*
	memory.cc
	---------
*/

#include "memory.hh"

// v68k-alloc
#include "v68k-alloc/memory.hh"

// v68k-mac
#include "v68k-mac/memory.hh"

// v68k-screen
#include "screen/storage.hh"

// xv68k
#include "screen.hh"


#pragma exceptions off


const uint32_t screen_addr = 0x0001A700;


memory_manager::memory_manager( uint8_t*  low_mem_base,
                                uint32_t  low_mem_size )
:
	its_low_mem_size( low_mem_size ),
	its_low_mem     ( low_mem_base, low_mem_size )
{
}

uint8_t* memory_manager::translate( uint32_t               addr,
                                    uint32_t               length,
                                    v68k::function_code_t  fc,
                                    v68k::memory_access_t  access ) const
{
	if ( addr >= v68k::alloc::start  &&  addr < v68k::alloc::limit )
	{
		return v68k::alloc::translate( addr, length, fc, access );
	}
	
	const uint32_t screen_size = v68k::screen::the_screen_size;
	
	if ( addr >= screen_addr  &&  addr < screen_addr + screen_size )
	{
		return screen::translate( addr - screen_addr, length, fc, access );
	}
	
	if ( addr < 3 * 1024  &&  (addr & 0x07FF) < 1024 )
	{
		if ( fc <= v68k::user_program_space  &&  access != v68k::mem_exec )
		{
			// Mac OS low memory
			return v68k::mac::translate( addr, length, fc, access );
		}
	}
	
	if ( addr < its_low_mem_size )
	{
		return its_low_mem.translate( addr, length, fc, access );
	}
	
	return its_callout_memory.translate( addr, length, fc, access );
}
