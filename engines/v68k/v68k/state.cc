/*
	state.cc
	--------
*/

#include "v68k/state.hh"

// v68k
#include "v68k/endian.hh"


namespace v68k
{
	
	processor_state::processor_state( processor_model model, const memory& mem )
	:
		mem( mem ),
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
	
	void processor_state::prefetch_instruction_word()
	{
		if ( regs.pc & 1 )
		{
			address_error();
		}
		else if ( !mem.get_instruction_word( regs.pc, opcode, program_space() ) )
		{
			bus_error();
		}
	}
	
	uint32_t processor_state::read_mem( uint32_t addr, op_size_t size )
	{
		if ( size != byte_sized  &&  badly_aligned_data( addr ) )
		{
			return address_error();
		}
		
		uint32_t result;
		
		bool ok;
		
		switch ( size )
		{
			case byte_sized:
				uint8_t byte;
				
				ok = mem.get_byte( addr, byte, data_space() );
				
				result = byte;
				
				break;
			
			case word_sized:
				uint16_t word;
				
				ok = mem.get_word( addr, word, data_space() );
				
				result = word;
				
				break;
			
			case long_sized:
				ok = mem.get_long( addr, result, data_space() );
				
				break;
			
			default:
				// Not reached
				break;
		}
		
		if ( !ok )
		{
			return bus_error();
		}
		
		return result;
	}
	
	uint16_t processor_state::get_CCR() const
	{
		const uint16_t ccr = regs.   x <<  4
		                   | regs.nzvc <<  0;
		
		return ccr;
	}
	
	uint16_t processor_state::get_SR() const
	{
		const uint16_t sr = regs.ttsm << 12
		                  | regs. iii <<  8
		                  | regs.   x <<  4
		                  | regs.nzvc <<  0;
		
		return sr;
	}
	
	void processor_state::set_CCR( uint16_t new_ccr )
	{
		// ...X NZVC  (all processors)
		
		regs.   x = new_ccr >>  4 & 0x1;
		regs.nzvc = new_ccr >>  0 & 0xF;
	}
	
	void processor_state::set_SR( uint16_t new_sr )
	{
		// T.S. .III ...X NZVC  (all processors)
		// .T.M .... .... ....  (68020 and later)
		
		const bool has_extra_bits = model >= mc68020;
		
		const uint16_t sr_mask = 0xA71F | 0x5000 & -has_extra_bits ;
		
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
		regs. iii = new_sr >>  8 & 0xF;
		regs.   x = new_sr >>  4 & 0xF;
		regs.nzvc = new_sr >>  0 & 0xF;
	}
	
	bool processor_state::take_exception_format_0( uint16_t vector_offset )
	{
		const uint16_t saved_sr = get_SR();
		
		set_SR( saved_sr & 0x3FFF | 0x2000 );  // Clear T1/T0, set S
		
		uint32_t& sp = regs.a[7];
		
		if ( badly_aligned_data( sp ) )
		{
			return address_error();
		}
		
		const uint32_t size = 8;
		
		sp -= size;
		
		const bool ok = mem.put_word( sp + 0, saved_sr,      data_space() )
		              & mem.put_long( sp + 2, regs.pc,       data_space() )
		              & mem.put_word( sp + 6, vector_offset, data_space() );  // format is 0
		
		if ( !ok )
		{
			return bus_error();
		}
		
		if ( badly_aligned_data( regs.vbr ) )
		{
			return address_error();
		}
		
		if ( !mem.get_long( regs.vbr + vector_offset, regs.pc, data_space() ) )
		{
			return bus_error();
		}
		
		prefetch_instruction_word();
		
		return true;
	}
	
	bool processor_state::take_exception_format_6( uint16_t vector_offset, uint32_t instruction_address )
	{
		const uint16_t saved_sr = get_SR();
		
		set_SR( saved_sr & 0x3FFF | 0x2000 );  // Clear T1/T0, set S
		
		uint32_t& sp = regs.a[7];
		
		if ( badly_aligned_data( sp ) )
		{
			return address_error();
		}
		
		const uint32_t size = 12;
		
		sp -= size;
		
		const uint32_t format_and_offset = 6 << 12 | vector_offset;
		
		const bool ok = mem.put_word( sp + 0, saved_sr,            data_space() )
		              & mem.put_long( sp + 2, regs.pc,             data_space() )
		              & mem.put_word( sp + 6, format_and_offset,   data_space() )
		              & mem.put_long( sp + 8, instruction_address, data_space() );
		
		if ( !ok )
		{
			return bus_error();
		}
		
		if ( badly_aligned_data( regs.vbr ) )
		{
			return address_error();
		}
		
		if ( !mem.get_long( regs.vbr + vector_offset, regs.pc, data_space() ) )
		{
			return bus_error();
		}
		
		prefetch_instruction_word();
		
		return true;
	}
	
}

