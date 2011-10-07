/*
	load_store.cc
	-------------
*/

#include "v68k/load_store.hh"

// v68k
#include "v68k/instruction.hh"
#include "v68k/state.hh"


namespace v68k
{
	
	#define N( x )  (!!(x) << 3)
	#define Z( x )  (!!(x) << 2)
	#define V( x )  (!!(x) << 1)
	#define C( x )  (!!(x) << 0)
	
	
	bool load( const processor_state& s, op_params& pb )
	{
		bool ok = true;
		
		const int32_t target = pb.target;
		
		if ( target >= 0 )
		{
			pb.second = s.regs.d[ target ];
			
			if ( target > 7 )
			{
				// address register, don't sign-extend
				return true;
			}
		}
		else
		{
			const uint32_t addr = pb.address;
			
			uint8_t   byte;
			uint16_t  word;
			
			switch ( pb.size )
			{
				case byte_sized:
					ok = s.mem.get_byte( addr, byte, s.data_space() );
					
					pb.second = byte;
					
					break;
				
				case word_sized:
					ok = s.mem.get_word( addr, word, s.data_space() );
					
					pb.second = word;
					
					break;
				
				case long_sized:
					ok = s.mem.get_long( addr, pb.second, s.data_space() );
					
					break;
				
				default:
					break;
			}
		}
		
		switch ( pb.size )
		{
			case byte_sized:
				pb.second = int32_t( int8_t( pb.second ) );
				break;
			
			case word_sized:
				pb.second = int32_t( int16_t( pb.second ) );
				break;
			
			default:
				break;
		}
		
		return ok;
	}
	
	bool store( processor_state& s, uint32_t loc, uint32_t data, int flags )
	{
		const int storage_flags = flags & stores_data_mask;
		
		if ( storage_flags == 0 )
		{
			return true;
		}
		
		if ( flags & in_register  &&  loc > 7 )
		{
			// Address register:  don't touch CCR
			
			s.regs.d[ loc ] = data;
			
			return true;
		}
		
		uint32_t data_mask;
		
		switch ( storage_flags )
		{
			case stores_byte_data:
				data_mask = 0x000000FF;
				break;
			
			case stores_word_data:
				data_mask = 0x0000FFFF;
				break;
			
			case stores_long_data:
				data_mask = 0xFFFFFFFF;
				break;
			
			default:
				break;
		}
		
		if ( flags & and_sets_CCR )
		{
			// CCR is already set
		}
		else
		{
			const uint32_t sign_mask = data_mask / 2 + 1;
			
			s.regs.nzvc = N( (data & sign_mask) != 0 )
			            | Z( (data & data_mask) == 0 )
			            | V( 0 )
			            | C( 0 );
		}
		
		if ( flags & in_register )
		{
			// loc is a register id
			
			s.regs.d[ loc ] &= ~data_mask;
			s.regs.d[ loc ] |=  data_mask & data;
			
			return true;
		}
		
		// loc is an address
		
		switch ( storage_flags )
		{
			case stores_byte_data:  return s.mem.put_byte( loc, data, s.data_space() );
			case stores_word_data:  return s.mem.put_word( loc, data, s.data_space() );
			case stores_long_data:  return s.mem.put_long( loc, data, s.data_space() );
			
			default:
				break;
		}
		
		return true;
	}
	
}

