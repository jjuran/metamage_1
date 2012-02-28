/*
	memory.cc
	---------
*/

// v68k-callbacks
#include "callback/bridge.hh"
#include "callback/memory.hh"


#pragma exceptions off


namespace v68k     {
namespace callback {

static uint8_t callback_trap[] = { 0x48, 0x4B, 0x48, 0x4B, 0x48 };  // BKPT #3 (2.5x)


uint8_t* memory::translate( uint32_t               addr,
                            uint32_t               length,
                            v68k::function_code_t  fc,
                            v68k::memory_access_t  access ) const
{
	if ( access >= v68k::mem_write )
	{
		// Callback memory is read-only
		
		return 0;  // NULL
	}
	
	const uint32_t first_callback_addr = 0 - n * sizeof (uint16_t);
	
	if ( addr < first_callback_addr )
	{
		// address out of bounds
		
		return 0;  // NULL
	}
	
	return callback_trap + (addr & 1);
}

}  // namespace v68k
}  // namespace callback

