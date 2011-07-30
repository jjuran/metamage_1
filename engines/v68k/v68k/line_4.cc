/*
	line_4.cc
	---------
*/

#include "v68k/line_4.hh"

// v68k
#include "v68k/instructions.hh"


#pragma exceptions off


namespace v68k
{
	
	static const instruction* decode_48( uint16_t opcode )
	{
		if ( (opcode & 0xFFF8) == 0x4808 )
		{
			return &decoded_LINK_L;
		}
		
		if ( (opcode & 0xFFF8) == 0x4848 )
		{
			return &decoded_BKPT;
		}
		
		if ( (opcode & 0xFFB8) == 0x4880 )
		{
			return opcode & 0x0040 ? &decoded_EXT_L
			                       : &decoded_EXT_W;
		}
		
		return 0;  // NULL
	}
	
	static const instruction* instructions_4e7_low[] =
	{
		0,  // RESET
		&decoded_NOP,
		&decoded_STOP,
		0,  // RTE
		0,  // RTD
		0,  // RTS
		0,  // TRAPV
		0   // RTR
	};
	
	static const instruction* decode_4e( uint16_t opcode )
	{
		if ( opcode & 0x0080 )
		{
			// JMP, JSR
			return 0;  // NULL
		}
		
		if ( opcode & 0x0040 )
		{
			switch ( opcode >> 3 & 0x7 )
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
					
					return &decoded_MOVE_to_USP;
				
				case 5:  // 0x4e68 - 0x4e6f
					
					return &decoded_MOVE_from_USP;
				
				case 6:  // 0x4e70 - 0x4e77
					
					// RESET, NOP, STOP, RTE, RTD, RTS, TRAPV, RTR
					return instructions_4e7_low[ opcode & 0x7 ];
				
				case 7:  // 0x4e78 - 0x4e7f
					
					// MOVEC
					break;
			}
		}
		
		return 0;  // NULL
	}
	
	const instruction* decode_line_4( uint16_t opcode )
	{
		if ( opcode & 0x0100 )
		{
			return 0;  // NULL
		}
		
		switch ( opcode & 0xff00 )
		{
			case 0x4800:
				return decode_48( opcode );
			
			case 0x4e00:
				return decode_4e( opcode );
			
			default:
				break;
		}
		
		return 0;  // NULL
	}
	
}

