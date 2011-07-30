/*
	line_4.cc
	---------
*/

#include "v68k/line_4.hh"

// v68k
#include "v68k/instructions.hh"
#include "v68k/registers.hh"


#pragma exceptions off


namespace v68k
{
	
	static const instruction* instructions_4e7_low[] =
	{
		0,  // RESET
		0,  // NOP
		0,  // STOP
		0,  // RTE
		0,  // RTD
		0,  // RTS
		0,  // TRAPV
		0   // RTR
	};
	
	static const instruction* decode_4e( const registers& regs, const memory& mem )
	{
		if ( regs.op & 0x0080 )
		{
			// JMP, JSR
			return 0;  // NULL
		}
		
		if ( regs.op & 0x0040 )
		{
			switch ( regs.op >> 3 & 0x7 )
			{
				case 0:  // 0x4e40 - 0x4e47
				case 1:  // 0x4e48 - 0x4e4f
					
					// TRAP
					break;
				
				case 2:  // 0x4e50 - 0x4e57
					
					return &decoded_LINK;
				
				case 3:  // 0x4e58 - 0x4e5f
					
					return &decoded_UNLK;
				
				case 4:  // 0x4e60 - 0x4e67
					
					// MOVE to USP
					break;
				
				case 5:  // 0x4e68 - 0x4e6f
					
					// MOVE from USP
					break;
				
				case 6:  // 0x4e70 - 0x4e77
					
					// RESET, NOP, STOP, RTE, RTD, RTS, TRAPV, RTR
					return instructions_4e7_low[ regs.op & 0x7 ];
				
				case 7:  // 0x4e78 - 0x4e7f
					
					// MOVEC
					break;
			}
		}
		
		return 0;  // NULL
	}
	
	const instruction* decode_line_4( const registers& regs, const memory& mem )
	{
		if ( regs.op & 0x0100 )
		{
			return 0;  // NULL
		}
		
		switch ( regs.op & 0xff00 )
		{
			case 0x4e00:
				return decode_4e( regs, mem );
			
			default:
				break;
		}
		
		return 0;  // NULL
	}
	
}

