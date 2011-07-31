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
		&decode_unimplemented,
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

