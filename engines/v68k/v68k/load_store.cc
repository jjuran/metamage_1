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
	
	
	bool load( const processor_state& s, uint32_t& param, int flags )
	{
		const int storage_flags = flags & stores_data_mask;
		
		if ( !(flags & loads_and) )
		{
			return true;
		}
		
		if ( flags & in_register )
		{
			param = s.regs.d[ param ];
			
			return true;
		}
		
		bool ok = true;
		
		uint8_t   byte;
		uint16_t  word;
		
		switch ( storage_flags )
		{
			case stores_byte_data:
				ok = s.mem.get_byte( param, byte, s.data_space() );
				
				param = byte;
				
				break;
			
			case stores_word_data:
				ok = s.mem.get_word( param, word, s.data_space() );
				
				param = word;
				
				break;
			
			case stores_long_data:
				ok = s.mem.get_long( param, param, s.data_space() );
				
				break;
			
			default:
				break;
		}
		
		return true;
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
		
		/*
			      storage_flags >> 8       E  (1, 2,  3)
			     (storage_flags >> 8) - 1  E  (0, 1,  2)
			1 << (storage_flags >> 8) - 1  E  (1, 2,  4)
			      
			             n_bytes - 1   E  (0, 1,  3)
			        8 * (n_bytes - 1)  E  (0, 8, 24)
			
			0x80 << 8 * (n_bytes - 1)  E  (0x80, 0x8000, 0x80000000)
			
			sign_mask * 2      E  (0x100, 0x10000, 0x00000000)
			sign_mask * 2 - 1  E  ( 0xFF,  0xFFFF, 0xFFFFFFFF)
		*/
		
		const int n_bytes = 1 << (storage_flags >> 8) - 1;
		
		const uint32_t sign_mask = 0x80 << 8 * (n_bytes - 1);
		
		const uint32_t data_mask = sign_mask * 2 - 1;
		
		if ( flags & and_sets_CCR )
		{
			// CCR is already set
		}
		else
		{
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

