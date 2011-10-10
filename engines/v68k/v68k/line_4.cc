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


#pragma exceptions off


namespace v68k
{
	
	static const instruction* decode_48( uint16_t opcode, instruction& storage )
	{
		if ( (opcode & 0xFFF8) == 0x4808 )
		{
			return &decoded_LINK_L;
		}
		
		if ( (opcode & 0xFFF8) == 0x4848 )
		{
			storage.fetch = fetches_data_at_0007;
			
			storage.code = microcode_BKPT;
			
			return &storage;
		}
		
		if ( (opcode & 0xFFC0) == 0x4840 )
		{
			const uint16_t mode = opcode >> 3 & 0x7;
			const uint16_t n    = opcode >> 0 & 0x7;
			
			if ( ea_is_control( mode, n ) )
			{
				return &decoded_PEA;
			}
		}
		
		if ( (opcode & 0xFFB8) == 0x4880 )
		{
			storage.fetch = fetches_data_at_0007;
			
			storage.code = opcode & 0x0040 ? microcode_EXT_L
			                               : microcode_EXT_W;
			
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
				return opcode & 0x0040 ? &decoded_JMP
				                       : &decoded_JSR;
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
		else
		{
			if ( ea_is_data_register( mode, n ) )
			{
				return opcode & 0x0200 ? &decoded_MOVE_from_SR_to_Dn
				                       : &decoded_MOVE_from_CCR_to_Dn;
			}
			else if ( ea_is_data_alterable( mode, n ) )
			{
				return opcode & 0x0200 ? &decoded_MOVE_from_SR
				                       : &decoded_MOVE_from_CCR;
			}
		}
		
		return 0;  // NULL
	}
	
	static const instruction* decode_unary_op( uint16_t opcode, instruction& storage )
	{
		const uint16_t size_code = opcode >> 6 & 0x3;
		
		const uint16_t mode = opcode >> 3 & 0x7;
		const uint16_t n    = opcode >> 0 & 0x7;
		
		const uint16_t selector = opcode >> 9 & 0x7;
		
		if ( selector == 5 )
		{
			if ( ea_is_valid( mode, n )  &&  (mode != 1 || size_code != 0) )
			{
				storage.fetch = fetches_TST;
				storage.code  = &microcode_CMP;
				
				if ( mode == 1  ||  mode == 7  &&  n >= 2 )
				{
					storage.flags = not_before_68020;
				}
				
				return &storage;
			}
		}
		else if ( ea_is_data_alterable( mode, n ) )
		{
			const bool in_reg = (opcode >> 3 & 0x7) < 2;
			
			const instruction_flags_t stores_data = instruction_flags_t( size_code + 1 << 8 );
			const instruction_flags_t destination = instruction_flags_t( in_register * in_reg );
			
			storage.flags = loads_and | stores_data | destination;
			
			switch ( selector )
			{
				case 0:
					// NEGX
					return 0;
				
				case 1:
					storage.fetch = fetches_CLR;
					storage.code  = &microcode_MOVE;
					
					return &storage;
				
				case 2:
					storage.fetch = fetches_CLR;
					storage.code  = &microcode_NEG;
					storage.flags = storage.flags | and_sets_CCR;
					
					return &storage;
				
				case 3:
					storage.fetch = fetches_NOT;
					storage.code  = &microcode_EOR;
					
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
			
			if ( (opcode & 0xF1C0) == 0x41C0 )
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
			
			if ( mode == update_mode  ||  ea_is_control( mode, n )  &&  (ea_is_alterable( mode, n ) || !to_mem ) )
			{
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
				return 0;  // NULL
			
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
		
		return 0;  // NULL
	}
	
}

