/*
	decode.cc
	---------
*/

#include "v68k/decode.hh"

// v68k
#include "v68k/ea_types.hh"
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
		
		if ( (opcode & 0xff00) == 0x0200 )
		{
			// ANDI
			
			if ( (opcode & 0xffbf ) == 0x023c )
			{
				return opcode & 0x0040 ? &decoded_ANDI_to_SR
				                       : &decoded_ANDI_to_CCR;
			}
		}
		
		return 0;  // NULL
	}
	
	static const instruction* move_instructions[] =
	{
		&decoded_MOVE_B_to_Dn,
		&decoded_MOVE_B,
		
		&decoded_MOVE_L_to_Rn,  // includes MOVEA.L
		&decoded_MOVE_L,
		
		&decoded_MOVE_W_to_Rn,  // includes MOVEA.W
		&decoded_MOVE_W
	};
	
	static const instruction* decode_MOVE( uint16_t opcode )
	{
		const uint16_t mode = opcode >> 3 & 0x7;
		const uint16_t n    = opcode >> 0 & 0x7;
		
		const uint16_t mode2 = opcode >> 6 & 0x7;
	//	const uint16_t n2    = opcode >> 9 & 0x7;
		
		if ( !ea_is_valid( mode, n )  ||  !ea_is_alterable( mode2 ) )
		{
			return 0;  // NULL
		}
		
		const uint16_t size_code = opcode >> 12 & 0x3;
		
		if ( size_code == 1  &&  (ea_is_address_register( mode )  ||  ea_is_address_register( mode2 )) )
		{
			return 0;  // NULL
		}
		
		const bool dest_code = !ea_is_register( mode2 );
		
		const int i = (size_code - 1) * 2 + dest_code;
		
		return move_instructions[ i ];
	}
	
	static const instruction* branch_instructions[] =
	{
		&decoded_BRA_S,
		&decoded_BRA,
		&decoded_BRA_L,
		
		&decoded_BSR_S,
		&decoded_BSR,
		&decoded_BSR_L,
		
		&decoded_Bcc_S,
		&decoded_Bcc,
		&decoded_Bcc_L
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
		&decode_MOVE,
		&decode_MOVE,
		&decode_MOVE,
		
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

