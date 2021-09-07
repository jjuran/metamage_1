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


#ifdef __MWERKS__
#pragma exceptions off
#endif


namespace v68k
{
	
	static
	op_result read_extended_displacement( processor_state&  s,
	                                      uint16_t          size_code,
	                                      int32_t&          displacement )
	{
		op_result result = Ok;
		
		uint32_t temp = 0;
		
		switch ( size_code )
		{
			case 2:
				result = fetch_instruction_word( s, low_word( temp ) );
				
				displacement = int16_t( temp );
				break;
			
			case 3:
				result = fetch_instruction_long( s, temp );
				// fall through
				
			default:
				displacement = temp;
				break;
		}
		
		// Assume null displacement on zero size code
		return result;
	}
	
	static
	op_result read_ea_displaced_address( processor_state& s, op_params& pb )
	{
		uint16_t displacement;
		
		op_result result = fetch_instruction_word( s, displacement );
		
		pb.address += int16_t( displacement );
		
		return result;
	}
	
	static
	op_result read_ea_indexed_address( processor_state& s, op_params& pb )
	{
		uint16_t extension;
		
		op_result result = fetch_instruction_word( s, extension );
		
		if ( result < 0 )
		{
			return result;
		}
		
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
			pb.address += index + base_displacement;
			
			return Ok;
		}
		
		const bool base_suppress  = extension & 0x80;
		
		const uint16_t bd_size = extension >> 4 & 0x3;
		
		result = read_extended_displacement( s, bd_size, base_displacement );
		
		if ( result < 0 )
		{
			return result;
		}
		
		if ( base_suppress )
		{
			pb.address = 0;
		}
		
		const bool index_suppress = extension & 0x40;
		
		if ( index_suppress )
		{
			index = 0;
		}
		
		const int iis = extension & 0x7;
		
		const bool postindexed = iis & 0x4;
		
		const bool memory_indirect = iis != 0;
		
		pb.address += base_displacement + !postindexed * index;
		
		if ( memory_indirect )
		{
			/*
				Don't check s.badly_aligned_data(), since Memory Indirect mode
				only occurs on 68020 or later models, which don't take exceptions
				for misaligned data accesses.
			*/
			
			if ( ! s.get_long( pb.address, pb.address, s.data_space() ) )
			{
				return Bus_error;
			}
			
			int32_t outer_displacement;
			
			result = read_extended_displacement( s, iis & 0x3, outer_displacement );
			
			if ( result < 0 )
			{
				return result;
			}
			
			pb.address += outer_displacement;
		}
		
		if ( postindexed )
		{
			pb.address += index;
		}
		
		return Ok;
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
				pb.address = An;
				
				return read_ea_displaced_address( s, pb );
			
			case 6:
				pb.address = An;
				
				return read_ea_indexed_address( s, pb );
				break;
			
			case 7:
				switch ( n )
				{
					case 0:
						return fetch_instruction_word_signed( s, pb.address );
					
					case 1:
						return fetch_instruction_long( s, pb.address );
					
					case 2:
						pb.address = s.pc();
						
						return read_ea_displaced_address( s, pb );
					
					case 3:
						pb.address = s.pc();
						
						return read_ea_indexed_address( s, pb );
					
					default:
						break;
				}
				
				break;
		}
		
		return Ok;
	}
	
}
