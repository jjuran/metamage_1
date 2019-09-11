/*
	VIA.cc
	------
*/

#include "VIA.hh"

// xv68k
#include "screen.hh"


namespace VIA {

static uint8_t VIA_reg_A = 0x48;  // use main screen and sound buffers
static uint8_t mmio_byte;

const uint32_t vBase  = 0xEFE1FE;
const uint32_t aVBufA = 0xEFFFFE;
const uint32_t aVBufB = vBase;

uint8_t* translate( addr_t addr, uint32_t length, fc_t fc, mem_t access )
{
	if ( length != 1 )
	{
		return 0;  // NULL
	}
	
	if ( access == v68k::mem_exec )
	{
		return 0;  // NULL
	}
	
	if ( access == v68k::mem_read )
	{
		switch ( addr )
		{
			case aVBufA:  return &VIA_reg_A;
			
			default:      return 0;  // NULL;
		}
	}
	
	if ( access == v68k::mem_update )
	{
		switch ( addr )
		{
			case aVBufA:
				if ( uint8_t diff = mmio_byte ^ VIA_reg_A )
				{
					if ( diff & 0x40 )
					{
						page_flip();
					}
					
					VIA_reg_A = mmio_byte;
				}
				break;
			
			default:
				break;
		}
	}
	
	return &mmio_byte;
}

}  // namespace VIA
