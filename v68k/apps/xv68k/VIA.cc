/*
	VIA.cc
	------
*/

#include "VIA.hh"

// xv68k
#include "screen.hh"


namespace VIA {

/*
	VIA Auxiliary Control Register (ACR):
	
	+----------+---------+---------+-----------------+---------+---------+
	| Bit #    |  7   6  |    5    |  4     3     2  |    1    |    0    |
	+----------+---------+---------+-----------------+---------+---------+
	|          |   T1    |   T2    | Shift Register  |   PB    |   PA    |
	| Function | Control | Control |     Control     |  Latch  |  Latch  |
	|          |         |         |                 | Enable  | Enable  |
	+----------+---------+---------+-----------------+---------+---------+
	
	(from the Synertek SY6522 manual)
*/

static uint8_t VIA_reg_A = 0x48;  // use main screen and sound buffers
static uint8_t VIA_reg_B = 0x80;  // sound disabled
static uint8_t VIA_ACR   = 0x00;  // One-shot; PB7, PB, PA, shift reg. disabled
static uint8_t mmio_byte;

enum
{
	vACR = 512 * 11,
};

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
			case aVBufB:  return &VIA_reg_B;
			
			case vBase + vACR:  return &VIA_ACR;
			
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
			
			case aVBufB:
				VIA_reg_B = mmio_byte;
				break;
			
			default:
				break;
		}
	}
	
	return &mmio_byte;
}

}  // namespace VIA
