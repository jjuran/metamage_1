/*
	decode.cc
	---------
*/

#include "v68k/decode.hh"

// v68k
#include "v68k/instructions.hh"
#include "v68k/memory.hh"
#include "v68k/registers.hh"


#pragma exceptions off


namespace v68k
{
	
	typedef const instruction* (*decoder)( const registers&, const memory& );
	
	
	static const instruction* decode_line_0( const registers& regs, const memory& mem )
	{
		if ( (regs.op & 0xf138) == 0x0108 )
		{
			return regs.op & 0x0080 ? &decoded_MOVEP_to
			                        : &decoded_MOVEP_from;
		}
		
		return 0;  // NULL
	}
	
	static const instruction* decode_line_7( const registers& regs, const memory& mem )
	{
		if ( regs.op & 0x0100 )
		{
			return 0;  // NULL
		}
		
		return &decoded_MOVEQ;
	}
	
	static const instruction* decode_line_C( const registers& regs, const memory& mem )
	{
		if ( regs.op & 0x0100 )
		{
			const uint16_t mode = regs.op >> 3 & 0x001f;
			
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
	
	static const instruction* decode_unimplemented( const registers& regs, const memory& mem )
	{
		return 0;  // NULL
	}
	
	static decoder decoder_per_line[] =
	{
		&decode_line_0,
		&decode_unimplemented,
		&decode_unimplemented,
		&decode_unimplemented,
		
		&decode_unimplemented,
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
	
	const instruction* decode( const registers& regs, const memory& mem )
	{
		return decoder_per_line[ regs.op >> 12 ]( regs, mem );
	}
	
}

