/*
	memory.cc
	---------
*/

#include "memory.hh"

// log-of-war
#include "logofwar/report.hh"

// v68k
#include "v68k/print.hh"

// v68k-alloc
#include "v68k-alloc/memory.hh"

// v68k-callouts
#include "callout/memory.hh"

// v68k-mac
#include "v68k-mac/memory.hh"

// v68k-screen
#include "screen/lock.hh"
#include "screen/storage.hh"
#include "screen/update.hh"

// xv68k
#include "screen.hh"
#include "sound.hh"
#include "VIA.hh"


#ifdef __MWERKS__
#pragma exceptions off
#endif


using v68k::addr_t;
using v68k::fc_t;
using v68k::mem_t;

using v68k::user_program_space;
using v68k::mem_write;
using v68k::mem_exec;


uint32_t alt_screen_addr  = 0x00012700;
uint32_t main_screen_addr = 0x0001A700;
uint32_t main_sound_addr  = 0x0001FD00;

const uint32_t sound_size = 740;

static uint8_t* low_memory_base;
static uint32_t low_memory_size;


static inline
bool vid_x2()
{
	return v68k::mac::get_CurPageOption() < 0;
}

static inline
bool addr_within_span( uint32_t addr, uint32_t base, uint32_t length )
{
	return addr >= base  &&  addr < base + length;
}

static inline
bool addr_in_screen( uint32_t addr, uint32_t length, uint32_t base )
{
	const uint32_t screen_size = v68k::screen::the_screen_size;
	
	return addr          <  base + screen_size  &&
	       addr + length <= base + screen_size  &&
	       addr          >= base;
}

static
uint8_t* lowmem_translate( addr_t addr, uint32_t length, fc_t fc, mem_t access )
{
	if ( addr < 1024 )
	{
		if ( fc <= user_program_space )
		{
			// No user access to system vectors
			
			return 0;  // NULL
		}
		
		if ( access == mem_exec )
		{
			// System vectors are not code -- not even the reset vector
			
			return 0;  // NULL
		}
	}
	
	if ( addr / 1024u == 7  &&  access == mem_write )
	{
		return 0;  // NULL
	}
	
	if ( length > low_memory_size  ||  addr > low_memory_size - length )
	{
		return 0;  // NULL
	}
	
	return low_memory_base + addr;
}

static
uint8_t* translate_with_diagnostic( uint32_t               addr,
                                    uint32_t               length,
                                    v68k::function_code_t  fc,
                                    v68k::memory_access_t  access )
{
	uint8_t* p = memory_manager::translate( addr, length, fc, access );
	
	if ( ! p )
	{
		WARNING = "Illegal memory access: ", length, "-byte ", access,
		" at ", v68k::hex32_t( addr ), " in ", fc;
	}
	
	return p;
}

memory_manager::memory_manager( uint8_t*  low_mem_base,
                                uint32_t  low_mem_size )
:
	v68k::memory( &translate_with_diagnostic )
{
	low_memory_base = low_mem_base;
	low_memory_size = low_mem_size;
}

uint8_t* memory_manager::translate( uint32_t               addr,
                                    uint32_t               length,
                                    v68k::function_code_t  fc,
                                    v68k::memory_access_t  access )
{
	if ( addr < 0xFFFF0000u )
	{
		/*
			24-bit mode compatibility hack for 32-bit-dirty applications
		*/
		
		addr &= 0x00FFFFFF;
	}
	
	if ( addr >= v68k::alloc::start  &&  addr < v68k::alloc::limit )
	{
		return v68k::alloc::translate( addr, length, fc, access );
	}
	
#ifndef __RELIX__
	
	/*
		In fixed-RAM configurations, the "low memory" segment is large
		enough to include the screen buffers.  Direct writes and reads of
		the screen there, and on update, copy to the live graphics buffer.
	*/
	
	if ( addr_in_screen( addr, length, main_screen_addr ) )
	{
		if ( access == mem_exec  ||  ! page_1_virtual_buffer )
		{
			return 0;  // NULL
		}
		
		using namespace v68k::screen;
		
		if ( is_unlocked()  &&  access == v68k::mem_update )
		{
			if ( virtual_buffer == page_1_virtual_buffer )
			{
				update();
			}
		}
		
		const uint32_t offset = addr - main_screen_addr;
		
		return page_1_virtual_buffer + offset;
	}
	
	if ( vid_x2()  &&  addr_in_screen( addr, length, alt_screen_addr ) )
	{
		if ( access == mem_exec  ||  ! page_2_virtual_buffer )
		{
			return 0;  // NULL
		}
		
		const uint32_t offset = addr - alt_screen_addr;
		
		return page_2_virtual_buffer + offset;
	}
	
	if ( addr_within_span( addr, main_sound_addr, sound_size ) )
	{
		return sound::translate( addr - main_sound_addr, length, fc, access );
	}
	
	if ( (addr >> 16) == 0x0040 )
	{
		// Mac ROM
		return v68k::mac::translate( addr, length, fc, access );
	}
	
#endif
	
	if ( addr < 3 * 1024  &&  (addr & 0x07FF) < 1024 )
	{
		if ( fc <= v68k::user_program_space  &&  access != v68k::mem_exec )
		{
			// Mac OS low memory
			return v68k::mac::translate( addr, length, fc, access );
		}
	}
	
	if ( addr < low_memory_size )
	{
		return lowmem_translate( addr, length, fc, access );
	}
	
#ifndef __RELIX__
	
	if ( (addr & 0x00FF0000) == 0xEF0000 )
	{
		return VIA::translate( addr, length, fc, access );
	}
	
#endif
	
	return v68k::callout::translate( addr, length, fc, access );
}
