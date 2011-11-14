/*
	fetch.cc
	--------
*/

#include "v68k/fetch.hh"

// v68k
#include "v68k/effective_address.hh"
#include "v68k/macros.hh"
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
	
	
	static void set_effective_address_param( processor_state& s, uint16_t mode, uint16_t n, op_params& pb )
	{
		const uint32_t ea = fetch_effective_address( s, mode, n, 1 << pb.size - 1 );
		
		(mode <= 1 ? pb.target : pb.address) = ea;
	}
	
	void fetch_effective_address( processor_state& s, op_params& pb )
	{
		const uint16_t mode = s.opcode >> 3 & 0x7;
		const uint16_t n    = s.opcode >> 0 & 0x7;
		
		set_effective_address_param( s, mode, n, pb );
	}
	
	void fetch_2nd_effective_address( processor_state& s, op_params& pb )
	{
		const uint16_t mode = s.opcode >> 6 & 0x7;
		const uint16_t n    = s.opcode >> 9 & 0x7;
		
		set_effective_address_param( s, mode, n, pb );
	}
	
	
	void fetch_zero( processor_state& s, op_params& pb )
	{
		pb.first = 0;
	}
	
	void fetch_ones( processor_state& s, op_params& pb )
	{
		pb.first = 0xFFFFFFFF;
	}
	
	void fetch_unsigned_word( processor_state& s, op_params& pb )
	{
		pb.first = fetch_instruction_word( s );
	}
	
	void fetch_signed_word( processor_state& s, op_params& pb )
	{
		const int16_t word = fetch_instruction_word( s );
		
		pb.first = int32_t( word );
	}
	
	uint32_t fetch_longword( processor_state& s )
	{
		const uint32_t high = fetch_instruction_word( s );
		
		return high << 16 | fetch_instruction_word( s );
	}
	
	
	void fetch_sized_immediate_data( processor_state& s, op_params& pb )
	{
		pb.first = pb.size == long_sized ? fetch_longword        ( s )
		                                 : fetch_instruction_word( s );
	}
	
	void fetch_sized_immediate_signed_data( processor_state& s, op_params& pb )
	{
		pb.first = pb.size == long_sized ? fetch_longword               ( s )
		                                 : fetch_instruction_word_signed( s );
	}
	
	void fetch_sized_data_at_effective_address( processor_state& s, op_params& pb )
	{
		const uint16_t mode = s.opcode >> 3 & 0x7;
		const uint16_t n    = s.opcode >> 0 & 0x7;
		
		if ( mode == 7  &&  n == 4 )
		{
			fetch_sized_immediate_data( s, pb );
			
			return;
		}
		
		const uint32_t addr = fetch_effective_address( s, mode, n, 1 << pb.size - 1 );
		
		const uint32_t data = mode <= 1 ? s.regs.d[ addr ]
		                                : s.read_mem( addr, pb.size );
		
		pb.first = sign_extend( data, pb.size );
	}
	
	void fetch_sized_data_from_major_register( processor_state& s, op_params& pb )
	{
		const uint16_t n = s.opcode >> 9 & 0x7;
		
		const uint32_t data = s.regs.d[n];
		
		pb.first = sign_extend( data, pb.size );
	}
	
	void fetch_bit_number_from_major_register( processor_state& s, op_params& pb )
	{
		const uint16_t n = s.opcode >> 9 & 0x7;
		
		const uint32_t data = s.regs.d[n];
		
		const uint32_t mask = s.opcode & 0x0038 ?  8 - 1   // memory
		                                        : 32 - 1;  // data register
		
		pb.first = data & mask;
	}
	
	void fetch_A_data_from_major_register( processor_state& s, op_params& pb )
	{
		const uint16_t n = s.opcode >> 9 & 0x7;
		
		const uint32_t data = s.regs.a[n];
		
		pb.second = s.regs.a[n];
	}
	
	
	void fetch_data_at_1E00( processor_state& s, op_params& pb )
	{
		pb.target = s.opcode >> 9 & 0x000F;
	}
	
	void fetch_data_at_000F( processor_state& s, op_params& pb )
	{
		pb.first = s.opcode & 0x000F;
	}
	
	
	void fetch_data_at_0E00( processor_state& s, op_params& pb )
	{
		pb.target = s.opcode >> 9 & 0x0007;
	}
	
	void fetch_data_at_0007( processor_state& s, op_params& pb )
	{
		pb.target = s.opcode & 0x0007;
	}
	
	
	void fetch_data_at_0001( processor_state& s, op_params& pb )
	{
		pb.second = s.opcode & 0x0001;
	}
	
	
	void fetch_MOVEP_address( processor_state& s, op_params& pb )
	{
		const uint32_t n = s.opcode & 0x7;
		
		const int32_t disp = fetch_instruction_word_signed( s );
		
		pb.address = s.regs.a[n] + disp;
	}
	
	
	void fetch_MOVEM_update( processor_state& s, op_params& pb )
	{
		const uint16_t mode = s.opcode >> 3 & 0x0007;
		
		if ( mode == 3  ||  mode == 4 )
		{
			pb.target = 0x8 | s.opcode & 0x7;  // register id for An
		}
		else
		{
			pb.target = 0;
		}
	}
	
	
	void fetch_ADDQ_data( processor_state& s, op_params& pb )
	{
		pb.first = ((s.opcode >> 9) - 1 & 0x0007) + 1;
	}
	
	
	void fetch_cc( processor_state& s, op_params& pb )
	{
		pb.second = s.opcode >> 8 & 0x0F;
	}
	
	
	void fetch_signed_data_at_00FF( processor_state& s, op_params& pb )
	{
		pb.first = int32_t( int8_t( s.opcode & 0x00ff ) );
	}
	
	
	void fetch_EXG_first_reg( processor_state& s, op_params& pb )
	{
		const uint32_t mode = s.opcode >> 3 & 0x001f;
		
		const uint32_t dA = (mode << 3) & mode;  // 0 for D or 8 for A
		
		pb.second = dA + (s.opcode & 0x0E00) >> 9;
	}
	
	
	void fetch_bit_shift_count( processor_state& s, op_params& pb )
	{
		const uint16_t n = s.opcode >> 9 & 0x7;
		
		/*
			The second branch below should be (n + 7) % 8 + 1, but MWC68K calls
			a generic mod function instead of optimizing it -- unlike the first
			branch -- so we do it by hand.
		*/
		
		pb.first = s.opcode & 0x0020 ? s.regs.d[n] % 64
		                             : (n - 1 & 8 - 1) + 1;
	}
	
	
	void assign_first_to_second( processor_state& s, op_params& pb )
	{
		pb.second = pb.first;
	}
	
	void add_first_to_address( processor_state& s, op_params& pb )
	{
		pb.address += int32_t( pb.first );
	}
	
	void add_X_to_first( processor_state& s, op_params& pb )
	{
		pb.first += s.regs.x & 0x1;
	}
	
	void shift_NEG_operands( processor_state& s, op_params& pb )
	{
		pb.first  = pb.second;
		pb.second = 0;
	}
	
	
	void read_address_on_68000( processor_state& s, op_params& pb )
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
	}
	
}

