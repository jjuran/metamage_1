/*
	state.cc
	--------
*/

#include "v68k/state.hh"


namespace v68k
{
	
	processor_state::processor_state( processor_model model, uint8_t* mem_base, uint32_t mem_size )
	:
		mem( mem_base, mem_size ),
		model( model ),
		condition()
	{
	}
	
	void processor_state::set_SR( uint16_t new_sr )
	{
		// T.S. .III ...X NZVC  (all processors)
		// .T.M .... .... ....  (68020 and later)
		
		const bool has_extra_bits = model >= mc68020;
		
		const uint16_t sr_mask = 0xA71F | 0x5000 * has_extra_bits ;
		
		new_sr &= sr_mask;
		
		if ( (regs.ttsm & 0x2) ^ (new_sr >> 12 & 2) )
		{
			// S changed:  swap SP with alt SP
			uint32_t& A7 = regs.a[7];
			
			const uint32_t temp = A7;
			
			A7 = regs.alt_sp;
			
			regs.alt_sp = temp;
		}
		
		if ( (regs.ttsm & 0x1) ^ (new_sr >> 12 & 1) )
		{
			// M changed:  swap SSP with alt SSP
			uint32_t& SSP = (new_sr >> 12 & 2) ? regs.a[7]
			                                   : regs.alt_sp;
			
			const uint32_t temp = SSP;
			
			SSP = regs.alt_ssp;
			
			regs.alt_ssp = temp;
		}
		
		regs.ttsm = new_sr >> 12;
		regs. iii = new_sr >>  8;
		regs.   x = new_sr >>  4;
		regs.nzvc = new_sr >>  0;
	}
	
}

