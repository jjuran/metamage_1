/*
	memory.cc
	---------
*/

// v68k-callouts
#include "callout/bridge.hh"
#include "callout/memory.hh"


#ifdef __MWERKS__
#pragma exceptions off
#endif


namespace v68k    {
namespace callout {

static uint8_t callout_trap[] = { 0x48, 0x4B, 0x48, 0x4B, 0x48 };  // BKPT #3 (2.5x)


uint8_t* translate( addr_t addr, uint32_t length, fc_t fc, mem_t access )
{
	if ( access >= v68k::mem_write )
	{
		// Callout memory is read-only
		
		return 0;  // NULL
	}
	
	/*
		Callout addresses occupy high memory.  Multiply the callout number by
		the size of the BKPT instruction to get the distance from the end of
		memory.  Subtract this from 0 and cast to uint32_t to get the address.
	*/
	
	const uint32_t first_callout_addr = uint32_t( 0 - n * sizeof (uint16_t) );
	
	if ( addr < first_callout_addr )
	{
		// address out of bounds
		
		return 0;  // NULL
	}
	
	return callout_trap + (addr & 1);
}

}  // namespace callout
}  // namespace v68k
