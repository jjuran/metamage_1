/*
	fetch.cc
	--------
*/

#include "v68k/fetch.hh"

// v68k
#include "v68k/effective_address.hh"
#include "v68k/state.hh"


namespace v68k
{
	
	uint16_t fetch_instruction_word( processor_state& s )
	{
		if ( s.regs.pc & 1 )
		{
			return s.address_error();
		}
		
		uint16_t word;
		
		if ( !s.mem.get_instruction_word( s.regs.pc, word, s.program_space() ) )
		{
			return s.bus_error();
		}
		
		s.regs.pc += 2;
		
		return word;
	}
	
	
	uint32_t fetch_zero( processor_state& s, int size_code )
	{
		return 0;
	}
	
	uint32_t fetch_ones( processor_state& s, int size_code )
	{
		return 0xFFFFFFFF;
	}
	
	uint32_t fetch_pc( processor_state& s, int size_code )
	{
		return s.regs.pc;
	}
	
	uint32_t fetch_unsigned_word( processor_state& s, int size_code )
	{
		return fetch_instruction_word( s );
	}
	
	uint32_t fetch_signed_word( processor_state& s, int size_code )
	{
		const int16_t word = fetch_instruction_word( s );
		
		return int32_t( word );
	}
	
	uint32_t fetch_longword( processor_state& s, int size_code )
	{
		const uint32_t high = fetch_instruction_word( s );
		
		return high << 16 | fetch_instruction_word( s );
	}
	
	
	uint32_t fetch_sized_immediate_data( processor_state& s, int size_code )
	{
		if ( size_code == 2 )
		{
			return fetch_longword( s );
		}
		
		return fetch_instruction_word( s );
	}
	
	uint32_t fetch_sized_effective_address( processor_state& s, int size_code )
	{
		switch ( size_code )
		{
			case 0:
				return fetch_effective_byte_address( s, 0 );
			
			case 1:
				return fetch_effective_word_address( s, 1 );
			
			case 2:
				return fetch_effective_long_address( s, 2 );
			
			default:
				// Not reached
				return 0;
		}
	}
	
	uint32_t fetch_MOVEM_sized_effective_address( processor_state& s, int size_code )
	{
		const int longword_sized = s.opcode >> 6 & 0x1;
		
		return longword_sized ? fetch_effective_long_address( s, 2 )
		                      : fetch_effective_word_address( s, 1 );
	}
	
	uint32_t fetch_sized_data_at_effective_address( processor_state& s, int size_code )
	{
		switch ( size_code )
		{
			case 0:
				return fetch_byte_from_effective_address( s );
			
			case 1:
				return fetch_word_from_effective_address( s );
			
			case 2:
				return fetch_long_from_effective_address( s );
			
			default:
				// Not reached
				return 0;
		}
	}
	
	uint32_t fetch_sized_data_from_major_register( processor_state& s, int size_code )
	{
		const uint16_t n = s.opcode >> 9 & 0x7;
		
		const uint32_t data = s.regs.d[n];
		
		switch ( size_code )
		{
			case 0:
				return int32_t( int8_t( data ) );
			
			case 1:
				return int32_t( int16_t( data ) );
			
			case 2:
				return data;
			
			default:
				// Not reached
				return 0;
		}
	}
	
	uint32_t fetch_bit_number_from_major_register( processor_state& s, int size_code )
	{
		const uint16_t n = s.opcode >> 9 & 0x7;
		
		const uint32_t data = s.regs.d[n];
		
		const uint32_t mask = s.opcode & 0x0038 ?  8 - 1   // memory
		                                        : 32 - 1;  // data register
		
		return data & mask;
	}
	
	uint32_t fetch_A_data_from_major_register( processor_state& s, int size_code )
	{
		const uint16_t n = s.opcode >> 9 & 0x7;
		
		const uint32_t data = s.regs.a[n];
		
		return s.regs.a[n];
	}
	
	uint32_t fetch_A_data_at_effective_address( processor_state& s, int size_code )
	{
		return s.opcode & 0x0100 ? fetch_long_from_effective_address( s )
		                         : fetch_word_from_effective_address( s );
	}
	
	
	uint32_t fetch_data_at_1E00( processor_state& s, int size_code )
	{
		return s.opcode >> 9 & 0x000F;
	}
	
	uint32_t fetch_data_at_000F( processor_state& s, int size_code )
	{
		return s.opcode & 0x000F;
	}
	
	
	uint32_t fetch_data_at_0E00( processor_state& s, int size_code )
	{
		return s.opcode >> 9 & 0x0007;
	}
	
	uint32_t fetch_data_at_0007( processor_state& s, int size_code )
	{
		return s.opcode & 0x0007;
	}
	
	uint32_t fetch_2nd_ea_register_id( processor_state& s, int size_code )
	{
		return (s.opcode >> 9 & 0x0007) | (s.opcode >> 3 & 1 << 3);
	}
	
	
	uint32_t fetch_MOVEP_opmode( processor_state& s, int size_code )
	{
		return s.opcode >> 6 & 0x0007;
	}
	
	
	uint32_t fetch_MOVEM_update( processor_state& s, int size_code )
	{
		const uint16_t mode = s.opcode >> 3 & 0x0007;
		
		if ( mode == 3  ||  mode == 4 )
		{
			return 0x8 | s.opcode & 0x7;  // register id for An
		}
		
		return 0;
	}
	
	
	uint32_t fetch_ADDQ_data( processor_state& s, int size_code )
	{
		return ((s.opcode >> 9) - 1 & 0x0007) + 1;
	}
	
	
	uint32_t fetch_cc( processor_state& s, int size_code )
	{
		return s.opcode >> 8 & 0x0F;
	}
	
	
	uint32_t fetch_signed_data_at_00FF( processor_state& s, int size_code )
	{
		return int32_t( int8_t( s.opcode & 0x00ff ) );
	}
	
	
	uint32_t fetch_EXG_opmode( processor_state& s, int size_code )
	{
		return s.opcode >> 3 & 0x001f;
	}
	
	
	uint32_t fetch_bit_shift_count( processor_state& s, int size_code )
	{
		const uint16_t n = s.opcode >> 9 & 0x7;
		
		/*
			The second branch below should be (n + 7) % 8 + 1, but MWC68K calls
			a generic mod function instead of optimizing it -- unlike the first
			branch -- so we do it by hand.
		*/
		
		return s.opcode & 0x0020 ? s.regs.d[n] % 64
		                         : (n - 1 & 8 - 1) + 1;
	}
	
}

