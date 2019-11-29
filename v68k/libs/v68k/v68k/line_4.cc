/*
	line_4.cc
	---------
*/

#include "v68k/line_4.hh"

// v68k
#include "v68k/ea_types.hh"
#include "v68k/fetches.hh"
#include "v68k/instructions.hh"
#include "v68k/microcode.hh"
#include "v68k/ops.hh"


#ifdef __MWERKS__
#pragma exceptions off
#endif


namespace v68k
{
	
	static const instruction* decode_48( uint16_t opcode, instruction& storage )
	{
		const uint16_t size_code = opcode >> 6 & 0x3;
		
		const uint16_t mode = opcode >> 3 & 0x7;
		const uint16_t n    = opcode >> 0 & 0x7;
		
		if ( size_code == 0 )
		{
			if ( mode == 1 )
			{
				return &decoded_LINK_L;
			}
			
			if ( ea_is_data_alterable( mode, n ) )
			{
				storage.size  = byte_sized;
				storage.fetch = fetches_NEGX;
				storage.code  = microcode_SBCD;
				storage.flags = stores_data | SUBX_CCR_update;
				
				return &storage;
			}
		}
		
		if ( size_code == 1 )
		{
			if ( mode <= 1 )
			{
				storage.fetch = fetches_data_at_0007;
				
				if ( mode == 0 )
				{
					storage.size  = long_sized;
					storage.code  = microcode_SWAP;
					storage.flags = basic_CCR_update;
				}
				else
				{
					storage.code = microcode_BKPT;
				}
				
				return &storage;
			}
			
			if ( ea_is_control( mode, n ) )
			{
				return &decoded_PEA;
			}
		}
		
		if ( size_code >= 2  &&  mode == 0 )
		{
			storage.fetch = fetches_data_at_0007;
			
			storage.code = size_code > 2 ? sign_extend_word
			                             : sign_extend_byte;
			
			storage.size = size_code > 2 ? long_sized
			                             : word_sized;
			
			storage.flags = loads_and | stores_data | basic_CCR_update;
			
			return &storage;
		}
		
		return 0;  // NULL
	}
	
	static const instruction* instructions_4e7_low[] =
	{
		&decoded_RESET,
		&decoded_NOP,
		&decoded_STOP,
		&decoded_RTE,
		&decoded_RTD,
		&decoded_RTS,
		&decoded_TRAPV,
		&decoded_RTR
	};
	
	static const instruction* decode_4e( uint16_t opcode, instruction& storage )
	{
		if ( opcode & 0x0080 )
		{
			const uint16_t mode = opcode >> 3 & 0x7;
			const uint16_t n    = opcode >> 0 & 0x7;
			
			if ( ea_is_control( mode, n ) )
			{
				storage.fetch = fetches_effective_address;
				
				storage.code = opcode & 0x0040 ? microcode_BRA
				                               : microcode_BSR;
				
				if ( ! (opcode & 0x0040) )
				{
					storage.flags = uses_stack;
				}
				
				return &storage;
			}
			
			return 0;  // NULL
		}
		
		if ( opcode & 0x0040 )
		{
			switch ( opcode >> 3 & 0x7 )
			{
				case 0:  // 0x4e40 - 0x4e47
				case 1:  // 0x4e48 - 0x4e4f
					
					return &decoded_TRAP;
				
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
					
					if ( (opcode & 0x0006) == 0x0002 )
					{
						return &decoded_MOVEC;
					}
					
					break;
			}
		}
		
		return 0;  // NULL
	}
	
	static const instruction* decode_MOVE_SR( uint16_t opcode, instruction& storage )
	{
		// MOVE from/to CCR/SR
		
		const uint16_t mode = opcode >> 3 & 0x7;
		const uint16_t n    = opcode >> 0 & 0x7;
		
		if ( opcode & 0x0400 )
		{
			if ( ea_is_data( mode, n ) )
			{
				return opcode & 0x0200 ? &decoded_MOVE_to_SR
				                       : &decoded_MOVE_to_CCR;
			}
		}
		else if ( ea_is_data_alterable( mode, n ) )
		{
			storage.size  = word_sized;
			storage.fetch = fetches_effective_address;
			
			storage.code = opcode & 0x0200 ? microcode_MOVE_from_CCR
			                               : microcode_MOVE_from_SR;
			
			storage.flags = opcode & 0x0200 ? stores_data | not_before_68010
			                                : stores_data | privileged_except_on_68000;
			
			return &storage;
		}
		
		return 0;  // NULL
	}
	
	static const instruction* decode_TAS( uint16_t opcode, instruction& storage )
	{
		const uint16_t mode = opcode >> 3 & 0x7;
		const uint16_t n    = opcode >> 0 & 0x7;
		
		if ( ea_is_data_alterable( mode, n ) )
		{
			storage.size  = byte_sized;
			storage.fetch = fetches_effective_address;
			storage.code  = &microcode_TAS;
			storage.flags = loads_and | stores_data;
			
			return &storage;
		}
		
		return 0;  // NULL
	}
	
	static const instruction* decode_unary_op( uint16_t opcode, instruction& storage )
	{
		storage.size = op_size_in_00C0;
		
		const uint16_t size_code = opcode >> 6 & 0x3;
		
		const uint16_t mode = opcode >> 3 & 0x7;
		const uint16_t n    = opcode >> 0 & 0x7;
		
		const uint16_t selector = opcode >> 9 & 0x7;
		
		if ( selector == 5 )
		{
			if ( ea_is_valid( mode, n )  &&  (mode != 1 || size_code != 0) )
			{
				storage.fetch = fetches_TST;
				storage.code  = &microcode_MOVE;
				storage.flags = TST_CCR_update;
				
				if ( mode == 1  ||  (mode == 7  &&  n >= 2) )
				{
					storage.flags = not_before_68020 | TST_CCR_update;
				}
				
				return &storage;
			}
		}
		else if ( ea_is_data_alterable( mode, n ) )
		{
			switch ( selector )
			{
				case 0:
					// NEGX
					storage.fetch = fetches_NEGX;
					storage.code  = &microcode_SUB;
					storage.flags = stores_data | SUBX_CCR_update;
					
					return &storage;
				
				case 1:
					storage.fetch = fetches_CLR;
					storage.code  = &microcode_MOVE;
					storage.flags = stores_data | basic_CCR_update;
					
					return &storage;
				
				case 2:
					storage.fetch = fetches_NEG;
					storage.code  = &microcode_SUB;
					storage.flags = stores_data | SUB_CCR_update;
					
					return &storage;
				
				case 3:
					storage.fetch = fetches_NOT;
					storage.code  = &microcode_EOR;
					storage.flags = loads_and | stores_data | basic_CCR_update;
					
					return &storage;
				
			}
		}
		
		return 0;  // NULL
	}
	
	const instruction* decode_line_4( uint16_t opcode, instruction& storage )
	{
		if ( opcode & 0x0100 )
		{
			if ( (opcode & 0xFFF8) == 0x49C0 )
			{
				return &decoded_EXTB;
			}
			
			if ( (opcode & 0x0040) == 0x0000 )
			{
				const uint16_t mode = opcode >> 3 & 0x7;
				const uint16_t n    = opcode >> 0 & 0x7;
				
				if ( ea_is_data( mode, n ) )
				{
					if ( opcode & 0x0080 )
					{
						storage.size  = word_sized;
					}
					else
					{
						storage.size  = long_sized;
						storage.flags = not_before_68020;
					}
					
					storage.fetch = fetches_CMP;
					storage.code  = microcode_CHK;
					
					return &storage;
				}
			}
			else if ( (opcode & 0xF1C0) == 0x41C0 )
			{
				const uint16_t mode = opcode >> 3 & 0x7;
				const uint16_t n    = opcode >> 0 & 0x7;
				
				if ( ea_is_control( mode, n ) )
				{
					return &decoded_LEA;
				}
			}
			
			return 0;  // NULL
		}
		
		if ( (opcode & 0xFB80) == 0x4880 )
		{
			const uint16_t mode = opcode >> 3 & 0x7;
			const uint16_t n    = opcode >> 0 & 0x7;
			
			const bool to_mem = !(opcode & 0x0400);
			
			const uint16_t update_mode = 3 + to_mem;  // postinc or predec
			
			if ( mode == update_mode  ||  (ea_is_control( mode, n )  &&  (ea_is_alterable( mode, n ) || !to_mem )) )
			{
				storage.size  = op_size_in_0040;
				storage.fetch = fetches_MOVEM;
				
				storage.code = to_mem ? &microcode_MOVEM_to
				                      : &microcode_MOVEM_from;
				
				return &storage;
			}
		}
		
		switch ( opcode & 0xff00 )
		{
			case 0x4800:
				return decode_48( opcode, storage );
			
			case 0x4c00:
				// MULL, DIVL (MOVEM handled above)
				storage.size  = long_sized;
				storage.code  = opcode & 0x0040 ? &microcode_DIV_L
				                                : &microcode_MUL_L;
				storage.fetch = fetches_word_and_EA;
				storage.flags = not_before_68020 | loads_and;
				
				return &storage;
			
			case 0x4e00:
				return decode_4e( opcode, storage );
			
			default:
				break;
		}
		
		if ( (opcode & 0x00C0) != 0x00C0 )
		{
			return decode_unary_op( opcode, storage );
		}
		
		if ( (opcode & 0x09C0) == 0x00C0 )
		{
			return decode_MOVE_SR( opcode, storage );
		}
		
		if ( (opcode & 0x0FC0) == 0x0AC0 )
		{
			return decode_TAS( opcode, storage );
		}
		
		return 0;  // NULL
	}
	
}
