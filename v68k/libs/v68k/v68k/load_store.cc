/*
	load_store.cc
	-------------
*/

#include "v68k/load_store.hh"

// v68k
#include "v68k/instruction.hh"
#include "v68k/macros.hh"
#include "v68k/state.hh"


#pragma exceptions off


namespace v68k
{
	
	#define N( x )  (!!(x) << 3)
	#define Z( x )  (!!(x) << 2)
	#define V( x )  (!!(x) << 1)
	#define C( x )  (!!(x) << 0)
	
	
	void load( processor_state& s, op_params& pb )
	{
		const int32_t target = pb.target;
		
		if ( target >= 0 )
		{
			pb.second = s.regs[ target ];
			
			if ( target > 7 )
			{
				// address register, don't sign-extend
				return;
			}
		}
		else
		{
			pb.second = s.read_mem( pb.address, pb.size );
		}
		
		pb.second = sign_extend( pb.second, pb.size );
	}
	
	bool store( processor_state& s, const op_params& pb )
	{
		if ( pb.size == unsized )
		{
			return true;
		}
		
		uint32_t data = pb.result;
		
		const int32_t target = pb.target;
		
		if ( target >= 0 )
		{
			if ( target <= 7 )
			{
				// Mask data registers only
				
				data = update( s.regs[ target ], data, pb.size );
			}
			
			s.regs[ target ] = data;
			
			return true;
		}
		
		// destination is a memory address
		
		const uint32_t addr = pb.address;
		
		switch ( pb.size )
		{
			case byte_sized:  return s.mem.put_byte( addr, data, s.data_space() );
			case word_sized:  return s.mem.put_word( addr, data, s.data_space() );
			case long_sized:  return s.mem.put_long( addr, data, s.data_space() );
			
			default:
				break;
		}
		
		return true;
	}
	
}

