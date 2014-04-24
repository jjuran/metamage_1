/*
	fetch.cc
	--------
*/

#include "v68k/fetch.hh"

// v68k
#include "v68k/conditional.hh"
#include "v68k/effective_address.hh"
#include "v68k/endian.hh"
#include "v68k/macros.hh"
#include "v68k/op_params.hh"
#include "v68k/state.hh"


#pragma exceptions off


namespace v68k
{
	
	op_result fetch_instruction_word( processor_state& s, uint16_t& word )
	{
		if ( s.pc() & 1 )
		{
			return Address_error;
		}
		
		if ( !s.mem.get_instruction_word( s.pc(), word, s.program_space() ) )
		{
			return Bus_error;
		}
		
		s.pc() += 2;
		
		return Ok;
	}
	
	op_result fetch_instruction_long( processor_state& s, uint32_t& longword )
	{
		op_result result;
		
		result = fetch_instruction_word( s, high_word( longword ) );  if ( result < 0 )  return result;
		result = fetch_instruction_word( s, low_word ( longword ) );  if ( result < 0 )  return result;
		
		return Ok;
	}
	
	uint16_t fetch_instruction_word( processor_state& s )
	{
		if ( s.pc() & 1 )
		{
			return s.address_error();
		}
		
		uint16_t word;
		
		if ( !s.mem.get_instruction_word( s.pc(), word, s.program_space() ) )
		{
			return s.bus_error();
		}
		
		s.pc() += 2;
		
		return word;
	}
	
	
	static op_result set_effective_address_param( processor_state& s, uint16_t mode, uint16_t n, op_params& pb )
	{
		const uint32_t ea = fetch_effective_address( s, mode, n, byte_count( pb.size ) );
		
		(mode <= 1 ? pb.target : pb.address) = ea;
		
		return Ok;
	}
	
	op_result fetch_effective_address( processor_state& s, op_params& pb )
	{
		const uint16_t mode = s.opcode >> 3 & 0x7;
		const uint16_t n    = s.opcode >> 0 & 0x7;
		
		return set_effective_address_param( s, mode, n, pb );
	}
	
	op_result fetch_2nd_effective_address( processor_state& s, op_params& pb )
	{
		const uint16_t mode = s.opcode >> 6 & 0x7;
		const uint16_t n    = s.opcode >> 9 & 0x7;
		
		return set_effective_address_param( s, mode, n, pb );
	}
	
	
	op_result fetch_zero( processor_state& s, op_params& pb )
	{
		pb.first = 0;
		
		return Ok;
	}
	
	op_result fetch_one( processor_state& s, op_params& pb )
	{
		pb.first = 1;
		
		return Ok;
	}
	
	op_result fetch_ones( processor_state& s, op_params& pb )
	{
		pb.first = 0xFFFFFFFF;
		
		return Ok;
	}
	
	op_result fetch_unsigned_word( processor_state& s, op_params& pb )
	{
		high_word( pb.first ) = 0;
		
		return fetch_instruction_word( s, low_word( pb.first ) );
	}
	
	op_result fetch_signed_word( processor_state& s, op_params& pb )
	{
		op_result result = fetch_instruction_word( s, low_word( pb.first ) );
		
		pb.first = int32_t( int16_t( pb.first ) );
		
		return result;
	}
	
	uint32_t fetch_longword( processor_state& s )
	{
		const uint32_t high = fetch_instruction_word( s );
		
		return high << 16 | fetch_instruction_word( s );
	}
	
	
	op_result fetch_sized_immediate_data( processor_state& s, op_params& pb )
	{
		pb.first = pb.size == long_sized ? fetch_longword        ( s )
		                                 : fetch_instruction_word( s );
		
		return Ok;
	}
	
	op_result fetch_sized_immediate_signed_data( processor_state& s, op_params& pb )
	{
		pb.first = pb.size == long_sized ? fetch_longword               ( s )
		                                 : fetch_instruction_word_signed( s );
		
		return Ok;
	}
	
	op_result fetch_sized_data_at_effective_address( processor_state& s, op_params& pb )
	{
		const uint16_t mode = s.opcode >> 3 & 0x7;
		const uint16_t n    = s.opcode >> 0 & 0x7;
		
		if ( mode == 7  &&  n == 4 )
		{
			fetch_sized_immediate_data( s, pb );
			
			return Ok;
		}
		
		const uint32_t addr = fetch_effective_address( s, mode, n, byte_count( pb.size ) );
		
		const uint32_t data = mode <= 1 ? s.regs[ addr ]
		                                : s.read_mem( addr, pb.size );
		
		pb.first = sign_extend( data, pb.size );
		
		return Ok;
	}
	
	op_result fetch_sized_data_from_major_register( processor_state& s, op_params& pb )
	{
		const uint16_t n = s.opcode >> 9 & 0x7;
		
		const uint32_t data = s.d(n);
		
		pb.first = sign_extend( data, pb.size );
		
		return Ok;
	}
	
	op_result fetch_bit_number_from_major_register( processor_state& s, op_params& pb )
	{
		const uint16_t n = s.opcode >> 9 & 0x7;
		
		const uint32_t data = s.d(n);
		
		const uint32_t mask = s.opcode & 0x0038 ?  8 - 1   // memory
		                                        : 32 - 1;  // data register
		
		pb.first = data & mask;
		
		return Ok;
	}
	
	op_result fetch_A_data_from_major_register( processor_state& s, op_params& pb )
	{
		const uint16_t n = s.opcode >> 9 & 0x7;
		
		const uint32_t data = s.a(n);
		
		pb.second = s.a(n);
		
		return Ok;
	}
	
	
	op_result fetch_data_at_1E00( processor_state& s, op_params& pb )
	{
		pb.target = s.opcode >> 9 & 0x000F;
		
		return Ok;
	}
	
	op_result fetch_data_at_000F( processor_state& s, op_params& pb )
	{
		pb.first = s.opcode & 0x000F;
		
		return Ok;
	}
	
	
	op_result fetch_data_at_0E00( processor_state& s, op_params& pb )
	{
		pb.target = s.opcode >> 9 & 0x0007;
		
		return Ok;
	}
	
	op_result fetch_data_at_0007( processor_state& s, op_params& pb )
	{
		pb.target = s.opcode & 0x0007;
		
		return Ok;
	}
	
	
	op_result fetch_data_at_0001( processor_state& s, op_params& pb )
	{
		pb.second = s.opcode & 0x0001;
		
		return Ok;
	}
	
	
	op_result fetch_MOVEP_address( processor_state& s, op_params& pb )
	{
		const uint32_t n = s.opcode & 0x7;
		
		pb.address = s.a(n);
		
		return Ok;
	}
	
	
	op_result fetch_MOVEM_update( processor_state& s, op_params& pb )
	{
		const uint16_t mode = s.opcode >> 3 & 0x0007;
		
		if ( mode == 3  ||  mode == 4 )
		{
			pb.target = 0x8 | (s.opcode & 0x7);  // register id for An
		}
		else
		{
			pb.target = 0;
		}
		
		return Ok;
	}
	
	
	op_result fetch_ADDQ_data( processor_state& s, op_params& pb )
	{
		pb.first = ((s.opcode >> 9) - 1 & 0x0007) + 1;
		
		return Ok;
	}
	
	
	op_result fetch_conditional( processor_state& s, op_params& pb )
	{
		uint16_t cc = s.opcode >> 8 & 0x0F;
		
		int32_t flag = int32_t() - test_conditional( cc, s.sr.nzvc );
		
		pb.result = flag;
		
		return Ok;
	}
	
	
	op_result fetch_signed_data_at_00FF( processor_state& s, op_params& pb )
	{
		pb.first = int32_t( int8_t( s.opcode & 0x00ff ) );
		
		return Ok;
	}
	
	
	op_result fetch_CMPM( processor_state& s, op_params& pb )
	{
		const uint32_t n1 = s.opcode >> 0 & 0x7;
		const uint32_t n2 = s.opcode >> 9 & 0x7;
		
		pb.first  = sign_extend( s.read_mem( s.a( n1 ), pb.size ), pb.size );
		pb.second = sign_extend( s.read_mem( s.a( n2 ), pb.size ), pb.size );
		
		const int increment = byte_count( pb.size );
		
		s.a( n1 ) += increment;
		s.a( n2 ) += increment;
		
		return Ok;
	}
	
	
	op_result fetch_EXG_first_reg( processor_state& s, op_params& pb )
	{
		const uint32_t mode = s.opcode >> 3 & 0x001f;
		
		const uint32_t dA = (mode << 3) & mode;  // 0 for D or 8 for A
		
		pb.second = dA | (s.opcode & 0x0E00) >> 9;
		
		return Ok;
	}
	
	
	op_result fetch_ADDX_predecrement( processor_state& s, op_params& pb )
	{
		const uint32_t i = (s.opcode & 0x0007) >> 0;
		const uint32_t j = (s.opcode & 0x0E00) >> 9;
		
		s.a(i) -= byte_count( pb.size );
		s.a(j) -= byte_count( pb.size );
		
		pb.first = s.read_mem( s.a(i), pb.size );
		
		pb.address = s.a(j);
		
		return Ok;
	}
	
	
	op_result fetch_bit_shift_count( processor_state& s, op_params& pb )
	{
		const uint16_t n = s.opcode >> 9 & 0x7;
		
		/*
			The second branch below should be (n + 7) % 8 + 1, but MWC68K calls
			a generic mod function instead of optimizing it -- unlike the first
			branch -- so we do it by hand.
		*/
		
		pb.first = s.opcode & 0x0020 ? s.d(n) % 64
		                             : (n - 1 & 8 - 1) + 1;
		
		return Ok;
	}
	
	
	op_result force_long_sized( processor_state& s, op_params& pb )
	{
		pb.size = long_sized;
		
		return Ok;
	}
	
	op_result assign_first_to_second( processor_state& s, op_params& pb )
	{
		pb.second = pb.first;
		
		return Ok;
	}
	
	op_result add_first_to_address( processor_state& s, op_params& pb )
	{
		pb.address += int32_t( pb.first );
		
		return Ok;
	}
	
	op_result add_X_to_first( processor_state& s, op_params& pb )
	{
		pb.first += s.sr.x & 0x1;
		
		return Ok;
	}
	
	op_result shift_NEG_operands( processor_state& s, op_params& pb )
	{
		pb.first  = pb.second;
		pb.second = 0;
		
		return Ok;
	}
	
	
	op_result read_address_on_68000( processor_state& s, op_params& pb )
	{
		/*
			This routine does nothing but make a memory read access on a 68000
			for strict compatibility.  It's needed by CLR.
			
			"In the MC68000 and MC68008 a memory location is read before it is
			cleared."
				-- M68000 Family Programmer's Reference Manual
		*/
		
		if ( s.model < mc68010 )
		{
			(void) s.read_mem( pb.address, pb.size );
		}
		
		return Ok;
	}
	
}
