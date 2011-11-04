/*
	fetch.cc
	--------
*/

#include "v68k/fetch.hh"

// v68k
#include "v68k/effective_address.hh"
#include "v68k/op_params.hh"
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
	
	
	static uint32_t set_effective_address_param( processor_state& s, uint16_t mode, uint16_t n, op_params& pb )
	{
		const uint32_t ea = fetch_effective_address( s, mode, n, 1 << pb.size - 1 );
		
		return (mode <= 1 ? pb.target : pb.address) = ea;
	}
	
	uint32_t fetch_effective_address( processor_state& s, op_params& pb )
	{
		const uint16_t mode = s.opcode >> 3 & 0x7;
		const uint16_t n    = s.opcode >> 0 & 0x7;
		
		return set_effective_address_param( s, mode, n, pb );
	}
	
	uint32_t fetch_2nd_effective_address( processor_state& s, op_params& pb )
	{
		const uint16_t mode = s.opcode >> 6 & 0x7;
		const uint16_t n    = s.opcode >> 9 & 0x7;
		
		return set_effective_address_param( s, mode, n, pb );
	}
	
	
	uint32_t fetch_zero( processor_state& s, op_params& pb )
	{
		return pb.first = 0;
	}
	
	uint32_t fetch_ones( processor_state& s, op_params& pb )
	{
		return pb.first = 0xFFFFFFFF;
	}
	
	uint32_t fetch_unsigned_word( processor_state& s, op_params& pb )
	{
		return pb.first = fetch_instruction_word( s );
	}
	
	uint32_t fetch_signed_word( processor_state& s, op_params& pb )
	{
		const int16_t word = fetch_instruction_word( s );
		
		return pb.first = int32_t( word );
	}
	
	uint32_t fetch_longword( processor_state& s )
	{
		const uint32_t high = fetch_instruction_word( s );
		
		return high << 16 | fetch_instruction_word( s );
	}
	
	
	uint32_t fetch_sized_immediate_data( processor_state& s, op_params& pb )
	{
		if ( pb.size == long_sized )
		{
			return pb.first = fetch_longword( s );
		}
		
		return pb.first = fetch_instruction_word( s );
	}
	
	uint32_t fetch_sized_immediate_signed_data( processor_state& s, op_params& pb )
	{
		if ( pb.size == long_sized )
		{
			return pb.first = fetch_longword( s );
		}
		
		return pb.first = fetch_instruction_word_signed( s );
	}
	
	uint32_t fetch_sized_data_at_effective_address( processor_state& s, op_params& pb )
	{
		const uint16_t mode = s.opcode >> 3 & 0x7;
		const uint16_t n    = s.opcode >> 0 & 0x7;
		
		if ( mode == 7  &&  n == 4 )
		{
			return fetch_sized_immediate_data( s, pb );
		}
		
		const uint32_t addr = fetch_effective_address( s, mode, n, 1 << pb.size - 1 );
		
		if ( (mode & 0x6) == 0 )
		{
			// 0 or 1
			
			const uint32_t data = s.regs.d[ addr ];
			
			switch ( pb.size )
			{
				case byte_sized:  return pb.first = int32_t( int8_t ( data ) );
				case word_sized:  return pb.first = int32_t( int16_t( data ) );
				case long_sized:  return pb.first =                   data;
			}
		}
		
		if ( pb.size != byte_sized  &&  s.badly_aligned_data( addr ) )
		{
			return s.address_error();
		}
		
		uint32_t result;
		
		bool ok;
		
		switch ( pb.size )
		{
			case byte_sized:
				uint8_t byte;
				
				ok = s.mem.get_byte( addr, byte, s.data_space() );
				
				result = int32_t( int8_t( byte ) );
				
				break;
			
			case word_sized:
				uint16_t word;
				
				ok = s.mem.get_word( addr, word, s.data_space() );
				
				result = int32_t( int16_t( word ) );
				
				break;
			
			case long_sized:
				ok = s.mem.get_long( addr, result, s.data_space() );
				
				break;
			
			default:
				// Not reached
				return 0;
		}
		
		if ( !ok )
		{
			return s.bus_error();
		}
		
		return pb.first = result;
	}
	
	uint32_t fetch_sized_data_from_major_register( processor_state& s, op_params& pb )
	{
		const uint16_t n = s.opcode >> 9 & 0x7;
		
		const uint32_t data = s.regs.d[n];
		
		switch ( pb.size )
		{
			case byte_sized:
				return pb.first = int32_t( int8_t( data ) );
			
			case word_sized:
				return pb.first = int32_t( int16_t( data ) );
			
			case long_sized:
				return pb.first = data;
			
			default:
				// Not reached
				return 0;
		}
	}
	
	uint32_t fetch_bit_number_from_major_register( processor_state& s, op_params& pb )
	{
		const uint16_t n = s.opcode >> 9 & 0x7;
		
		const uint32_t data = s.regs.d[n];
		
		const uint32_t mask = s.opcode & 0x0038 ?  8 - 1   // memory
		                                        : 32 - 1;  // data register
		
		return pb.first = data & mask;
	}
	
	uint32_t fetch_A_data_from_major_register( processor_state& s, op_params& pb )
	{
		const uint16_t n = s.opcode >> 9 & 0x7;
		
		const uint32_t data = s.regs.a[n];
		
		return pb.second = s.regs.a[n];
	}
	
	
	uint32_t fetch_data_at_1E00( processor_state& s, op_params& pb )
	{
		return pb.target = s.opcode >> 9 & 0x000F;
	}
	
	uint32_t fetch_data_at_000F( processor_state& s, op_params& pb )
	{
		return pb.first = s.opcode & 0x000F;
	}
	
	
	uint32_t fetch_data_at_0E00( processor_state& s, op_params& pb )
	{
		return pb.target = s.opcode >> 9 & 0x0007;
	}
	
	uint32_t fetch_data_at_0007( processor_state& s, op_params& pb )
	{
		return pb.target = s.opcode & 0x0007;
	}
	
	
	uint32_t fetch_data_at_0001( processor_state& s, op_params& pb )
	{
		return pb.second = s.opcode & 0x0001;
	}
	
	
	uint32_t fetch_MOVEP_address( processor_state& s, op_params& pb )
	{
		const uint32_t n = s.opcode & 0x7;
		
		const int32_t disp = fetch_instruction_word_signed( s );
		
		return pb.address = s.regs.a[n] + disp;
	}
	
	
	uint32_t fetch_MOVEM_update( processor_state& s, op_params& pb )
	{
		const uint16_t mode = s.opcode >> 3 & 0x0007;
		
		if ( mode == 3  ||  mode == 4 )
		{
			return pb.target = 0x8 | s.opcode & 0x7;  // register id for An
		}
		
		return pb.target = 0;
	}
	
	
	uint32_t fetch_ADDQ_data( processor_state& s, op_params& pb )
	{
		return pb.first = ((s.opcode >> 9) - 1 & 0x0007) + 1;
	}
	
	
	uint32_t fetch_cc( processor_state& s, op_params& pb )
	{
		return pb.second = s.opcode >> 8 & 0x0F;
	}
	
	
	uint32_t fetch_signed_data_at_00FF( processor_state& s, op_params& pb )
	{
		return pb.first = int32_t( int8_t( s.opcode & 0x00ff ) );
	}
	
	
	uint32_t fetch_EXG_first_reg( processor_state& s, op_params& pb )
	{
		const uint32_t mode = s.opcode >> 3 & 0x001f;
		
		const uint32_t dA = (mode << 3) & mode;  // 0 for D or 8 for A
		
		return pb.second = dA + (s.opcode & 0x0E00) >> 9;
	}
	
	
	uint32_t fetch_bit_shift_count( processor_state& s, op_params& pb )
	{
		const uint16_t n = s.opcode >> 9 & 0x7;
		
		/*
			The second branch below should be (n + 7) % 8 + 1, but MWC68K calls
			a generic mod function instead of optimizing it -- unlike the first
			branch -- so we do it by hand.
		*/
		
		pb.first = s.opcode & 0x0020 ? s.regs.d[n] % 64
		                             : (n - 1 & 8 - 1) + 1;
		
		return pb.first;
	}
	
	
	uint32_t assign_first_to_second( processor_state& s, op_params& pb )
	{
		return pb.second = pb.first;
	}
	
}

