/*
	decode.cc
	---------
*/

#include "v68k/decode.hh"

// v68k
#include "v68k/ea_types.hh"
#include "v68k/fetches.hh"
#include "v68k/instructions.hh"
#include "v68k/line_4.hh"
#include "v68k/microcode.hh"


#pragma exceptions off


namespace v68k
{
	
	typedef const instruction* (*decoder)( uint16_t opcode, instruction& storage );
	
	
	static const instruction* decode_line_0( uint16_t opcode, instruction& storage )
	{
		if ( (opcode & 0xf138) == 0x0108 )
		{
			return opcode & 0x0080 ? &decoded_MOVEP_to
			                       : &decoded_MOVEP_from;
		}
		
		const uint16_t mode = opcode >> 3 & 0x7;
		const uint16_t n    = opcode >> 0 & 0x7;
		
		if ( (opcode & 0xff00) == 0x0000 )
		{
			// ORI
			
			if ( (opcode & 0xffbf ) == 0x003c )
			{
				return opcode & 0x0040 ? &decoded_ORI_to_SR
				                       : &decoded_ORI_to_CCR;
			}
		}
		
		if ( (opcode & 0xff00) == 0x0200 )
		{
			// ANDI
			
			if ( (opcode & 0x00c0) != 0x00c0  &&  ea_is_data_alterable( mode, n ) )
			{
				const int size_code = opcode >> 6 & 0x3;
				
				const bool to_data = ea_is_data_register( mode );
				
				const instruction_flags_t stores_data = instruction_flags_t( size_code + 1 << 8 );
				const instruction_flags_t destination = instruction_flags_t( in_register * to_data );
				
				storage.fetch = fetches_immediate;
				storage.code  = &microcode_AND;
				storage.flags = loads_and | stores_data | destination;
				
				return &storage;
			}
			
			if ( (opcode & 0xffbf ) == 0x023c )
			{
				return opcode & 0x0040 ? &decoded_ANDI_to_SR
				                       : &decoded_ANDI_to_CCR;
			}
		}
		
		if ( (opcode & 0xff00) == 0x0a00 )
		{
			// EORI
			
			if ( (opcode & 0xffbf ) == 0x0a3c )
			{
				return opcode & 0x0040 ? &decoded_EORI_to_SR
				                       : &decoded_EORI_to_CCR;
			}
		}
		
		if ( (opcode & 0xff00) == 0x0e00 )
		{
			const int size_code = opcode >> 6 & 0x3;
			
			if ( size_code != 3  &&  ea_is_memory_alterable( mode ) )
			{
				return &decoded_MOVES;
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
	
	static const instruction* decode_MOVE( uint16_t opcode, instruction& storage )
	{
		const uint16_t mode = opcode >> 3 & 0x7;
		const uint16_t n    = opcode >> 0 & 0x7;
		
		const uint16_t mode2 = opcode >> 6 & 0x7;
		const uint16_t n2    = opcode >> 9 & 0x7;
		
		if ( !ea_is_valid( mode, n )  ||  !ea_is_alterable( mode2, n2 ) )
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
	
	static const instruction* decode_line_5( uint16_t opcode, instruction& storage )
	{
		const uint16_t size_code = opcode >> 6 & 0x3;
		
		const uint16_t mode = opcode >> 3 & 0x7;
		const uint16_t n    = opcode >> 0 & 0x7;
		
		if ( size_code != 3 )
		{
			if ( ea_is_alterable( mode, n ) )
			{
				if ( mode == 1 )
				{
					if ( size_code == 0 )
					{
						return 0;  // NULL
					}
					
					storage.code = opcode & 0x0100 ? &microcode_SUBA : &microcode_ADDA;
				}
				else
				{
					storage.code = opcode & 0x0100 ? &microcode_SUB : &microcode_ADD;
				}
				
				const bool to_reg = ea_is_register( mode );
				
				const instruction_flags_t stores_data = instruction_flags_t( size_code + 1 << 8 );
				const instruction_flags_t destination = instruction_flags_t( in_register * to_reg );
				
				storage.fetch = fetches_ADDQ;
				storage.flags = loads_and | stores_data | destination | and_sets_CCR;
				
				return &storage;
			}
		}
		else if ( ea_is_data_alterable( mode, n ) )
		{
			const bool to_reg = ea_is_register( mode );
			
			const instruction_flags_t destination = instruction_flags_t( in_register * to_reg );
			
			storage.fetch = fetches_Scc;
			storage.code  = &microcode_Scc;
			storage.flags = stores_byte_data | destination | and_sets_CCR;
			
			return &storage;
		}
		
		return 0;  // NULL
	}
	
	static const instruction* decode_line_6( uint16_t opcode, instruction& storage )
	{
		switch ( opcode & 0x00FF )
		{
			default:
				storage.fetch = fetches_branch_short;
				break;
			
			case 0x00:
				storage.fetch = fetches_branch;
				break;
			
			case 0xFF:
				storage.fetch = fetches_branch_long;
				storage.flags = not_before_68020;
				break;
		}
		
		switch ( opcode & 0xFF00 )
		{
			case 0x6000:
				storage.code = &microcode_BRA;
				break;
			
			case 0x6100:
				storage.code = &microcode_BSR;
				break;
			
			default:
				storage.code = &microcode_Bcc;
				break;
		}
		
		return &storage;
	}
	
	static const instruction* decode_line_7( uint16_t opcode, instruction& storage )
	{
		if ( opcode & 0x0100 )
		{
			return 0;  // NULL
		}
		
		return &decoded_MOVEQ;
	}
	
	static const instruction* decode_line_8( uint16_t opcode, instruction& storage )
	{
		const uint16_t size_code = opcode >> 6 & 0x3;
		
		const uint16_t mode = opcode >> 3 & 0x7;
		const uint16_t n    = opcode >> 0 & 0x7;
		
		const bool has_0100 = opcode & 0x0100;
		
		if ( size_code != 3  &&  (has_0100 ? ea_is_memory_alterable( mode ) : ea_is_data( mode, n )) )
		{
			const instruction_flags_t stores_data = instruction_flags_t( size_code + 1 << 8 );
			const instruction_flags_t destination = instruction_flags_t( in_register * !has_0100 );
			
			storage.fetch = has_0100 ? fetches_math : fetches_math_to_Dn;
			storage.code  = &microcode_OR;
			storage.flags = loads_and | stores_data | destination;
			
			return &storage;
		}
		
		return 0;  // NULL
	}
	
	static const instruction* decode_line_9( uint16_t opcode, instruction& storage )
	{
		const uint16_t size_code = opcode >> 6 & 0x3;
		
		const uint16_t mode = opcode >> 3 & 0x7;
		const uint16_t n    = opcode >> 0 & 0x7;
		
		const bool has_0100 = opcode & 0x0100;
		
		const bool is_SUB = size_code == 3 ? false
		                  : has_0100       ? ea_is_memory_alterable( mode )
		                  : size_code == 0 ? ea_is_data ( mode, n )
		                  :                  ea_is_valid( mode, n );
		
		if ( is_SUB )
		{
			const instruction_flags_t stores_data = instruction_flags_t( size_code + 1 << 8 );
			const instruction_flags_t destination = instruction_flags_t( in_register * !has_0100 );
			
			storage.fetch = has_0100 ? fetches_ADD : fetches_ADD_to_Dn;
			storage.code  = &microcode_SUB;
			storage.flags = loads_and | stores_data | destination | and_sets_CCR;
			
			return &storage;
		}
		else if ( size_code == 3 )
		{
			const instruction_flags_t stores_data = instruction_flags_t( (opcode & 0x0100) + 0x0200 );
			
			storage.fetch = fetches_ADDA;
			storage.code  = &microcode_SUBA;
			storage.flags = loads_and | stores_data | in_register;
			
			return &storage;
		}
		
		return 0;  // NULL
	}
	
	static const instruction* decode_line_B( uint16_t opcode, instruction& storage )
	{
		const uint16_t size_code = opcode >> 6 & 0x3;
		
		const uint16_t mode = opcode >> 3 & 0x7;
		const uint16_t n    = opcode >> 0 & 0x7;
		
		const bool has_0100 = opcode & 0x0100;
		
		if ( size_code == 3 )
		{
			if ( ea_is_valid( mode, n ) )
			{
				storage.fetch = fetches_CMPA;
				storage.code  = microcode_CMP;
				
				return &storage;
			}
		}
		else
		{
			if ( opcode & 0x0100 )
			{
				if ( ea_is_memory_alterable( mode ) )
				{
					const instruction_flags_t stores_data = instruction_flags_t( size_code + 1 << 8 );
					
					storage.fetch = fetches_math;
					storage.code  = &microcode_EOR;
					storage.flags = loads_and | stores_data;
					
					return &storage;
				}
			}
			else
			{
				if ( ea_is_valid( mode, n ) )
				{
					storage.fetch = fetches_CMP;
					storage.code  = microcode_CMP;
					
					return &storage;
				}
			}
		}
		
		return 0;  // NULL
	}
	
	static const instruction* decode_line_C( uint16_t opcode, instruction& storage )
	{
		const uint16_t size_code = opcode >> 6 & 0x3;
		
		const uint16_t mode = opcode >> 3 & 0x7;
		const uint16_t n    = opcode >> 0 & 0x7;
		
		const bool has_0100 = opcode & 0x0100;
		
		if ( size_code != 3  &&  (has_0100 ? ea_is_memory_alterable( mode ) : ea_is_data( mode, n )) )
		{
			const instruction_flags_t stores_data = instruction_flags_t( size_code + 1 << 8 );
			const instruction_flags_t destination = instruction_flags_t( in_register * !has_0100 );
			
			storage.fetch = has_0100 ? fetches_math : fetches_math_to_Dn;
			storage.code  = &microcode_AND;
			storage.flags = loads_and | stores_data | destination;
			
			return &storage;
		}
		
		if ( has_0100 )
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
	
	static const instruction* decode_line_D( uint16_t opcode, instruction& storage )
	{
		const uint16_t size_code = opcode >> 6 & 0x3;
		
		const uint16_t mode = opcode >> 3 & 0x7;
		const uint16_t n    = opcode >> 0 & 0x7;
		
		const bool has_0100 = opcode & 0x0100;
		
		const bool is_ADD = size_code == 3 ? false
		                  : has_0100       ? ea_is_memory_alterable( mode )
		                  : size_code == 0 ? ea_is_data ( mode, n )
		                  :                  ea_is_valid( mode, n );
		
		if ( is_ADD )
		{
			const instruction_flags_t stores_data = instruction_flags_t( size_code + 1 << 8 );
			const instruction_flags_t destination = instruction_flags_t( in_register * !has_0100 );
			
			storage.fetch = has_0100 ? fetches_ADD : fetches_ADD_to_Dn;
			storage.code  = &microcode_ADD;
			storage.flags = loads_and | stores_data | destination | and_sets_CCR;
			
			return &storage;
		}
		else if ( size_code == 3 )
		{
			const instruction_flags_t stores_data = instruction_flags_t( (opcode & 0x0100) + 0x0200 );
			
			storage.fetch = fetches_ADDA;
			storage.code  = &microcode_ADDA;
			storage.flags = loads_and | stores_data | in_register;
			
			return &storage;
		}
		
		return 0;  // NULL
	}
	
	static const microcode bit_shift_microcodes[] =
	{
		&microcode_ASR,
		&microcode_ASL,
		&microcode_LSR,
		&microcode_LSL
	};
	
	static const instruction* decode_line_E( uint16_t opcode, instruction& storage )
	{
		const uint16_t size_code = opcode >> 6 & 0x3;
		
		if ( size_code != 3  &&  (opcode & 0x0010) == 0x0000 )
		{
			const int i = (opcode & 0x0008) >> 2 | (opcode & 0x0100) >> 8;
			
			storage.code = bit_shift_microcodes[ i ];
			
			const instruction_flags_t stores_data = instruction_flags_t( size_code + 1 << 8 );
			
			storage.fetch = fetches_bit_shift;
			storage.flags = loads_and | stores_data | in_register | and_sets_CCR;
			
			return &storage;
		}
		
		return 0;  // NULL
	}
	
	static const instruction* decode_unimplemented( uint16_t opcode, instruction& storage )
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
		&decode_line_5,
		&decode_line_6,
		&decode_line_7,
		
		&decode_line_8,
		&decode_line_9,
		&decode_unimplemented,
		&decode_line_B,
		
		&decode_line_C,
		&decode_line_D,
		&decode_line_E,
		&decode_unimplemented
	};
	
	const instruction* decode( uint16_t opcode, instruction& storage )
	{
		return decoder_per_line[ opcode >> 12 ]( opcode, storage );
	}
	
}

