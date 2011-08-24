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
	
	
	uint32_t fetch_pc( processor_state& s )
	{
		return s.regs.pc;
	}
	
	uint32_t fetch_unsigned_word( processor_state& s )
	{
		return fetch_instruction_word( s );
	}
	
	uint32_t fetch_signed_word( processor_state& s )
	{
		const int16_t word = fetch_instruction_word( s );
		
		return int32_t( word );
	}
	
	uint32_t fetch_longword( processor_state& s )
	{
		const uint32_t high = fetch_instruction_word( s );
		
		return high << 16 | fetch_instruction_word( s );
	}
	
	
	uint32_t fetch_size_code( processor_state& s )
	{
		return s.opcode >> 6 & 0x3;
	}
	
	uint32_t fetch_sized_immediate_data( processor_state& s )
	{
		const int size_code = s.opcode >> 6 & 0x3;
		
	//	ASSERT( size_code != 3 );
		
		if ( size_code == 2 )
		{
			return fetch_longword( s );
		}
		
		return fetch_instruction_word( s );
	}
	
	uint32_t fetch_sized_effective_address( processor_state& s )
	{
		const int size_code = s.opcode >> 6 & 0x3;
		
	//	ASSERT( size_code != 3 );
		
		switch ( size_code )
		{
			case 0:
				return fetch_effective_byte_address( s );
			
			case 1:
				return fetch_effective_word_address( s );
			
			case 2:
				return fetch_effective_long_address( s );
			
			default:
				// Not reached
				return 0;
		}
	}
	
	uint32_t fetch_sized_data_at_effective_address( processor_state& s )
	{
		const int size_code = s.opcode >> 6 & 0x3;
		
	//	ASSERT( size_code != 3 );
		
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
	
	uint32_t fetch_sized_data_from_major_register( processor_state& s )
	{
		const uint16_t n = s.opcode >> 9 & 0x7;
		
		const uint32_t data = s.regs.d[n];
		
		const int size_code = s.opcode >> 6 & 0x3;
		
	//	ASSERT( size_code != 3 );
		
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
	
	uint32_t fetch_A_data_from_major_register( processor_state& s )
	{
		const uint16_t n = s.opcode >> 9 & 0x7;
		
		const uint32_t data = s.regs.a[n];
		
		return s.regs.a[n];
	}
	
	uint32_t fetch_A_data_at_effective_address( processor_state& s )
	{
		return s.opcode & 0x0100 ? fetch_long_from_effective_address( s )
		                         : fetch_word_from_effective_address( s );
	}
	
	
	uint32_t fetch_data_at_1E00( processor_state& s )
	{
		return s.opcode >> 9 & 0x000F;
	}
	
	uint32_t fetch_data_at_000F( processor_state& s )
	{
		return s.opcode & 0x000F;
	}
	
	
	uint32_t fetch_data_at_0E00( processor_state& s )
	{
		return s.opcode >> 9 & 0x0007;
	}
	
	uint32_t fetch_data_at_0007( processor_state& s )
	{
		return s.opcode & 0x0007;
	}
	
	uint32_t fetch_2nd_ea_register_id( processor_state& s )
	{
		return (s.opcode >> 9 & 0x0007) | (s.opcode >> 3 & 1 << 3);
	}
	
	
	uint32_t fetch_MOVEP_opmode( processor_state& s )
	{
		return s.opcode >> 6 & 0x0007;
	}
	
	
	uint32_t fetch_cc( processor_state& s )
	{
		return s.opcode >> 8 & 0x0F;
	}
	
	
	uint32_t fetch_signed_data_at_00FF( processor_state& s )
	{
		return int32_t( int8_t( s.opcode & 0x00ff ) );
	}
	
	
	uint32_t fetch_EXG_opmode( processor_state& s )
	{
		return s.opcode >> 3 & 0x001f;
	}
	
}

