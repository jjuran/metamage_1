/*
	state.cc
	--------
*/

#include "v68k/state.hh"

// v68k
#include "v68k/endian.hh"


#ifdef __MWERKS__
#pragma exceptions off
#endif


namespace v68k
{
	
	processor_state::processor_state( processor_model model, const memory& mem, bkpt_handler bkpt )
	:
		mem( mem ),
		bkpt( bkpt ),
		model( model ),
		condition()
	{
		uint32_t* p   = (uint32_t*)  &regs;
		uint32_t* end = (uint32_t*) (&regs + 1);
		
		for ( ;  p < end;  ++p )
		{
			*p = 0;
		}
	}
	
	op_result processor_state::read_byte( uint32_t addr, uint32_t& data )
	{
		if ( ! get_byte( addr, low_byte( low_word( data ) ), data_space() ) )
		{
			return Bus_error;
		}
		
		return Ok;
	}
	
	op_result processor_state::read_word( uint32_t addr, uint32_t& data )
	{
		if ( badly_aligned_data( addr ) )
		{
			return Address_error;
		}
		
		if ( ! get_word( addr, low_word( data ), data_space() ) )
		{
			return Bus_error;
		}
		
		return Ok;
	}
	
	op_result processor_state::read_long( uint32_t addr, uint32_t& data )
	{
		if ( badly_aligned_data( addr ) )
		{
			return Address_error;
		}
		
		if ( ! get_long( addr, data, data_space() ) )
		{
			return Bus_error;
		}
		
		return Ok;
	}
	
	uint16_t processor_state::get_CCR() const
	{
		const uint16_t ccr = sr.   x <<  4
		                   | sr.nzvc <<  0;
		
		return ccr;
	}
	
	uint16_t processor_state::get_SR() const
	{
		const uint16_t result = sr.ttsm << 12
		                      | sr. iii <<  8
		                      | sr.   x <<  4
		                      | sr.nzvc <<  0;
		
		return result;
	}
	
	void processor_state::set_CCR( uint16_t new_ccr )
	{
		// ...X NZVC  (all processors)
		
		sr.   x = new_ccr >>  4 & 0x1;
		sr.nzvc = new_ccr >>  0 & 0xF;
	}
	
	void processor_state::set_SR( uint16_t new_sr )
	{
		// T.S. .III ...X NZVC  (all processors)
		// .T.M .... .... ....  (68020 and later)
		
		const bool has_extra_bits = model >= mc68020;
		
		const uint16_t sr_mask = 0xA71F | (0x5000 & -has_extra_bits);
		
		new_sr &= sr_mask;
		
		save_sp();
		
		sr.ttsm = new_sr >> 12;
		sr. iii = new_sr >>  8 & 0xF;
		sr.   x = new_sr >>  4 & 0xF;
		sr.nzvc = new_sr >>  0 & 0xF;
		
		load_sp();
	}
	
}
