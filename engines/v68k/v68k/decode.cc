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
	
	
	static fetcher* bit_op_fetchers[] =
	{
		fetches_static_bit_op,
		fetches_dynamic_bit_op
	};
	
	static const microcode bit_op_microcodes[] =
	{
		&microcode_BCHG,
		&microcode_BCLR,
		&microcode_BSET,
		&microcode_BTST
	};
	
	static const microcode modify_SR_microcodes[] =
	{
		&microcode_ORI_to_CCR,
		&microcode_ORI_to_SR,
		
		&microcode_ANDI_to_CCR,
		&microcode_ANDI_to_SR,
		
		0,  // NULL
		0,  // NULL
		
		&microcode_EORI_to_CCR,
		&microcode_EORI_to_SR
	};
	
	static const microcode immediate_microcodes[] =
	{
		&microcode_OR,
		&microcode_AND,
		&microcode_SUB,
		&microcode_ADD,
		0,  // NULL
		&microcode_EOR,
		&microcode_CMP
	//	0  // NULL
	};
	
	static const instruction* decode_line_0( uint16_t opcode, instruction& storage )
	{
		if ( (opcode & 0xf138) == 0x0108 )
		{
			storage.size  = op_size_in_0040;
			storage.fetch = fetches_MOVEP;
			
			storage.code = opcode & 0x0080 ? microcode_MOVEP_to
			                               : microcode_MOVEP_from;
			
			return &storage;
		}
		
		const uint16_t mode = opcode >> 3 & 0x7;
		const uint16_t n    = opcode >> 0 & 0x7;
		
		const bool to_data = ea_is_data_register( mode );
		
		if ( opcode & 0x0100  ||  (opcode & 0xff00) == 0x0800 )
		{
			// bit ops
			
			const int i = opcode >> 6 & 0x3;
			
			if ( i != 0  &&  !ea_is_alterable( mode, n ) )
			{
				return 0;  // NULL
			}
			
			const int j = (opcode & 0x0100) >> 8;
			
			const instruction_flags_t stores_data = to_data ? stores_long_data : stores_byte_data;
			
			storage.size = to_data ? long_sized : byte_sized;
			
			storage.code  = bit_op_microcodes[ i ];
			storage.fetch = bit_op_fetchers  [ j ];
			storage.flags = loads_and | stores_data | and_sets_CCR;
			
			return &storage;
		}
		
		if ( (opcode & 0x05bf) == 0x003c )
		{
			storage.size = op_size_t( ((opcode & 0x0040) >> 6) + 1 );
			
			storage.fetch = fetches_unsigned_word;
			
			if ( opcode & 0x0040 )
			{
				storage.flags = privileged;
			}
			
			const int i = (opcode & 0x0040) >> 6
			            | (opcode & 0x0200) >> 8
			            | (opcode & 0x0800) >> 9;
			
			storage.code = modify_SR_microcodes[ i ];
			
			return &storage;
		}
		
		const int size_code = opcode >> 6 & 0x3;
		
		if ( size_code != 3 )
		{
			if ( (opcode & 0xff00) == 0x0e00 )
			{
				return ea_is_memory_alterable( mode ) ? &decoded_MOVES
				                                      : 0;  // NULL
			}
			
			const int selector = (opcode & 0x0E00) >> 9;
			
			if ( !ea_is_data( mode, n )  ||  selector != 6  &&  !ea_is_alterable( mode, n ) )
			{
				return 0;  // NULL
			}
			
			const instruction_flags_t stores_data = instruction_flags_t( size_code + 1 << 8 );
			
			storage.size = op_size_in_00C0;
			
			storage.code = immediate_microcodes[ selector ];
			
			storage.fetch = fetches_immediate;
			storage.flags = loads_and | stores_data;
			
			if ( selector & 2 )
			{
				storage.flags |= and_sets_CCR;
			}
			
			if ( selector == 6  &&  mode == 7  &&  n & 2 )
			{
				storage.flags |= not_before_68010;
			}
			
			return &storage;
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
				
				const instruction_flags_t stores_data = instruction_flags_t( size_code + 1 << 8 );
				
				storage.size  = op_size_in_00C0;
				storage.fetch = fetches_ADDQ;
				storage.flags = loads_and | stores_data | and_sets_CCR;
				
				return &storage;
			}
		}
		else if ( ea_is_data_alterable( mode, n ) )
		{
			storage.size  = byte_sized;
			storage.fetch = fetches_Scc;
			storage.code  = &microcode_Scc;
			storage.flags = stores_byte_data | and_sets_CCR;
			
			return &storage;
		}
		
		return 0;  // NULL
	}
	
	static const instruction* decode_line_6( uint16_t opcode, instruction& storage )
	{
		switch ( opcode & 0x00FF )
		{
			default:
				storage.size  = byte_sized;
				storage.fetch = fetches_branch_short;
				break;
			
			case 0x00:
				storage.size  = word_sized;
				storage.fetch = fetches_branch;
				break;
			
			case 0xFF:
				storage.size  = long_sized;
				storage.fetch = fetches_branch;
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
			
			storage.size  = op_size_in_00C0;
			storage.fetch = has_0100 ? fetches_EOR : fetches_math_to_Dn;
			storage.code  = &microcode_OR;
			storage.flags = loads_and | stores_data;
			
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
			
			storage.size  = op_size_in_00C0;
			storage.fetch = has_0100 ? fetches_ADD : fetches_ADD_to_Dn;
			storage.code  = &microcode_SUB;
			storage.flags = loads_and | stores_data | and_sets_CCR;
			
			return &storage;
		}
		else if ( size_code == 3 )
		{
			const instruction_flags_t stores_data = instruction_flags_t( (opcode & 0x0100) + 0x0200 );
			
			storage.size  = op_size_in_0100;
			storage.fetch = fetches_ADDA;
			storage.code  = &microcode_SUBA;
			storage.flags = loads_and | stores_data;
			
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
				storage.size  = op_size_in_0100;
				storage.fetch = fetches_CMPA;
				storage.code  = microcode_CMP;
				
				return &storage;
			}
		}
		else
		{
			storage.size = op_size_in_00C0;
			
			if ( opcode & 0x0100 )
			{
				if ( ea_is_data_alterable( mode, n ) )
				{
					const instruction_flags_t stores_data = instruction_flags_t( size_code + 1 << 8 );
					
					storage.fetch = fetches_EOR;
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
			
			storage.size  = op_size_in_00C0;
			storage.fetch = has_0100 ? fetches_EOR : fetches_math_to_Dn;
			storage.code  = &microcode_AND;
			storage.flags = loads_and | stores_data;
			
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
			
			storage.size  = op_size_in_00C0;
			storage.fetch = has_0100 ? fetches_ADD : fetches_ADD_to_Dn;
			storage.code  = &microcode_ADD;
			storage.flags = loads_and | stores_data | and_sets_CCR;
			
			return &storage;
		}
		else if ( size_code == 3 )
		{
			const instruction_flags_t stores_data = instruction_flags_t( (opcode & 0x0100) + 0x0200 );
			
			storage.size  = op_size_in_0100;
			storage.fetch = fetches_ADDA;
			storage.code  = &microcode_ADDA;
			storage.flags = loads_and | stores_data;
			
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
			
			storage.size  = op_size_in_00C0;
			storage.fetch = fetches_bit_shift;
			storage.flags = loads_and | stores_data | and_sets_CCR;
			
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

