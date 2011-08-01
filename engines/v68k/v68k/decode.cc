/*
	decode.cc
	---------
*/

#include "v68k/decode.hh"

// v68k
#include "v68k/instructions.hh"
#include "v68k/line_4.hh"


#pragma exceptions off


namespace v68k
{
	
	typedef const instruction* (*decoder)( uint16_t opcode );
	
	
	static const instruction* decode_line_0( uint16_t opcode )
	{
		if ( (opcode & 0xf138) == 0x0108 )
		{
			return opcode & 0x0080 ? &decoded_MOVEP_to
			                       : &decoded_MOVEP_from;
		}
		
		return 0;  // NULL
	}
	
	static const instruction* branch_instructions[] =
	{
		&decoded_BRA_S,
		&decoded_BRA,
		&decoded_BRA_L,
		
		&decoded_BSR_S,
		&decoded_BSR,
		&decoded_BSR_L,
		
		0,  // &decoded_Bcc_S
		0,  // &decoded_Bcc
		0   // &decoded_Bcc_L
	};
	
	static const instruction* decode_line_6( uint16_t opcode )
	{
		int size_log2;
		
		switch ( opcode & 0x00FF )
		{
			default:
				size_log2 = 0;  // byte
				break;
			
			case 0x00:
				size_log2 = 1;  // word
				break;
			
			case 0xFF:
				size_log2 = 2;  // long
				break;
		}
		
		int selector;
		
		switch ( opcode & 0xFF00 )
		{
			case 0x6000:
				selector = 0;  // BRA
				break;
			
			case 0x6100:
				selector = 1;  // BSR
				break;
			
			default:
				selector = 2;  // Bcc
				break;
		}
		
		const int i = selector * 3 + size_log2;
		
		return branch_instructions[ i ];
	}
	
	static const instruction* decode_line_7( uint16_t opcode )
	{
		if ( opcode & 0x0100 )
		{
			return 0;  // NULL
		}
		
		return &decoded_MOVEQ;
	}
	
	static const instruction* decode_line_C( uint16_t opcode )
	{
		if ( opcode & 0x0100 )
		{
			const uint16_t mode = opcode >> 3 & 0x001f;
			
			switch ( mode )
			{
				case 0x08:
				case 0x09:
				case 0x11:
					return &decoded_EXG;
			}
		}
		
		return 0;  // NULL
	}
	
	static const instruction* decode_unimplemented( uint16_t opcode )
	{
		return 0;  // NULL
	}
	
	static decoder decoder_per_line[] =
	{
		&decode_line_0,
		&decode_unimplemented,
		&decode_unimplemented,
		&decode_unimplemented,
		
		&decode_line_4,
		&decode_unimplemented,
		&decode_line_6,
		&decode_line_7,
		
		&decode_unimplemented,
		&decode_unimplemented,
		&decode_unimplemented,
		&decode_unimplemented,
		
		&decode_line_C,
		&decode_unimplemented,
		&decode_unimplemented,
		&decode_unimplemented
	};
	
	const instruction* decode( uint16_t opcode )
	{
		return decoder_per_line[ opcode >> 12 ]( opcode );
	}
	
}

