/*
	memory.cc
	---------
*/

#include "memory.hh"


#pragma exceptions off


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
	if ( addr < 3 * 1024  &&  (addr & 0x07FF) < 1024 )
	{
		if ( fc <= v68k::user_program_space  &&  access != v68k::mem_exec )
		{
			// Mac OS low memory
			return its_mac_low_mem.translate( addr, length, fc, access );
		}
	}
	
	if ( addr < its_low_mem_size )
	{
		return its_low_mem.translate( addr, length, fc, access );
	}
	
	if ( addr >= screen_addr  &&  addr < screen_addr + screen_size )
	{
		return its_screen.translate( addr - screen_addr, length, fc, access );
	}
	
	if ( addr >= v68k::alloc::start  &&  addr < v68k::alloc::limit )
	{
		return its_alloc_mem.translate( addr, length, fc, access );
	}
	
	return its_callback_memory.translate( addr, length, fc, access );
}

