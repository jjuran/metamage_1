/*
	effective_address.cc
	--------------------
*/

#include "v68k/effective_address.hh"

// v68k
#include "v68k/endian.hh"
#include "v68k/fetch.hh"
#include "v68k/macros.hh"
#include "v68k/state.hh"


#pragma exceptions off


namespace v68k
{
	
	static int32_t read_extended_displacement( processor_state& s, uint16_t size_code )
	{
		uint32_t displacement = 0;
		
		switch ( size_code )
		{
		//	case 1:  return 0;
			
			case 2:
				fetch_instruction_word( s, low_word( displacement ) );
				
				return int16_t( displacement );
			
			case 3:
				fetch_instruction_long( s, displacement );
				
				return displacement;
		}
		
		// Assume null displacement on zero size code
		return 0;
	}
	
	static uint32_t read_ea_displaced_address( processor_state& s, uint32_t address )
	{
		return address + fetch_instruction_word_signed( s );
	}
	
	static uint32_t read_ea_indexed_address( processor_state& s, uint32_t address )
	{
		const uint16_t extension = fetch_instruction_word( s );
		
		int32_t base_displacement = int8_t( extension & 0xff );
		
		const uint16_t _n = extension >> 12;
		
		// _n == 0- 7:  D0-D7
		// _n == 8-15:  A0-A7
		
		int32_t index = s.regs[ _n ];  // Xn
		
		const bool long_sized = extension >> 11 & 0x1;
		
		if ( !long_sized )
		{
			index = int16_t( index );
		}
		
		if ( s.model >= mc68020 )
		{
			const uint16_t scale_bits = extension >> 9 & 0x3;
			
			index <<= scale_bits;
		}
		
		const bool full_format = extension & 0x0100;
		
		if ( s.model < mc68020  ||  !full_format )
		{
			return address + index + base_displacement;
		}
		
		const bool base_suppress  = extension & 0x80;
		
		const uint16_t bd_size = extension >> 4 & 0x3;
		
		base_displacement = read_extended_displacement( s, bd_size ) & -!base_suppress;
		
		const bool index_suppress = extension & 0x40;
		
		if ( index_suppress )
		{
			index = 0;
		}
		
		const int iis = extension & 0x7;
		
		const bool postindexed = iis & 0x4;
		
		const bool memory_indirect = iis != 0;
		
		address += base_displacement + !postindexed * index;
		
		if ( memory_indirect )
		{
			/*
				Don't check s.badly_aligned_data(), since Memory Indirect mode
				only occurs on 68020 or later models, which don't take exceptions
				for misaligned data accesses.
			*/
			
			if ( !s.mem.get_long( address, address, s.data_space() ) )
			{
				return s.bus_error();
			}
			
			const int32_t outer_displacement = read_extended_displacement( s, iis & 0x3 );
			
			address += outer_displacement;
		}
		
		if ( postindexed )
		{
			address += index;
		}
		
		return address;
	}
	
	static inline uint32_t postincrement( uint32_t& An, int size )
	{
		const uint32_t result = An;
		
		An += size;
		
		return result;
	}
	
	static inline uint32_t predecrement( uint32_t& An, int size )
	{
		return An -= size;
	}
	
	op_result fetch_effective_address( processor_state& s, uint16_t mode, uint16_t n, op_params& pb )
	{
		const uint32_t An = s.a(n);
		
		int n_bytes;
		
		if ( mode == 3  ||  mode == 4 )
		{
		//	ASSERT( size != 0 );
			
			if ( pb.size == byte_sized  &&  n == 7 )
			{
				n_bytes = 2;  // SP is kept word-aligned
			}
			else
			{
				n_bytes = byte_count( pb.size );
			}
		}
		
		switch ( mode )
		{
			case 0:  // Data
			case 1:  // Address
				// Return a register id for Data/Address Register Direct mode.
				
				pb.target = mode << 3 | n;
				break;
			
			case 2:
				pb.address = An;
				break;
			
			case 3:
				pb.address = postincrement( s.a(n), n_bytes );
				break;
			
			case 4:
				pb.address = predecrement( s.a(n), n_bytes );
				break;
			
			case 5:
				pb.address = read_ea_displaced_address( s, An );
				break;
			
			case 6:
				pb.address = read_ea_indexed_address( s, An );
				break;
			
			case 7:
				switch ( n )
				{
					case 0:
						return fetch_instruction_word_signed( s, pb.address );
					
					case 1:
						return fetch_instruction_long( s, pb.address );
					
					case 2:
						pb.address = read_ea_displaced_address( s, s.pc() );
						break;
					
					case 3:
						pb.address = read_ea_indexed_address( s, s.pc() );
						break;
					
					default:
						break;
				}
				
				break;
		}
		
		return Ok;
	}
	
}
