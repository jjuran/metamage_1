/*
	emulator.cc
	-----------
*/

#include "v68k/emulator.hh"

// v68k
#include "v68k/decode.hh"
#include "v68k/endian.hh"
#include "v68k/instruction.hh"


namespace v68k
{
	
	struct reset_vector
	{
		uint32_t isp;
		uint32_t pc;
	};
	
	
	emulator::emulator( processor_model model, uint8_t* mem_base, uint32_t mem_size )
	:
		processor_state( model, mem_base, mem_size )
	{
	}
	
	void emulator::prefetch_instruction_word()
	{
		if ( regs.pc & 1 )
		{
			address_error();
		}
		else if ( !mem.get_instruction_word( regs.pc, opcode ) )
		{
			bus_error();
		}
	}
	
	void emulator::double_bus_fault()
	{
		condition = halted;
	}
	
	void emulator::reset()
	{
		condition = normal;
		
		try
		{
			const reset_vector* v = (const reset_vector*) mem.translate( 0, sizeof (reset_vector) );
			
			regs.ttsm = 0 << 2  // clear Trace bits
					  | 1 << 1  // set Supervisor bit
					  | 0;      // clear Master bit
			
			regs. iii = 7;  // set max Interrupt mask
			
			regs.   x = 0;  // clear CCR
			regs.nzvc = 0;
			
			regs.a[7] = longword_from_big( v->isp );
			regs.pc   = longword_from_big( v->pc  );
			
			// prefetch
			prefetch_instruction_word();
		}
		catch ( ... )
		{
			double_bus_fault();
			
			return;
		}
	}
	
	bool emulator::step()
	{
		if ( at_breakpoint() )
		{
			// Unacknowledged breakpoint traps as illegal instruction
			condition = halted;
		}
		
		if ( condition != normal )
		{
			return false;
		}
		
		// advance pc
		regs.pc += 2;
		
		// decode (prefetched)
		const instruction* decoded = decode( opcode );
		
		if ( !decoded )
		{
			return illegal_instruction();
		}
		
		if ( (decoded->flags & not_before_mask) > model )
		{
			return illegal_instruction();
		}
		
		if ( (decoded->flags & privilege_mask) > ((regs.ttsm & 0x2) | (model == mc68000)) )
		{
			return privilege_violation();
		}
		
		// prepare
		fetcher* fetch = decoded->fetch;
		
		uint32_t params[ max_params ];
		
		uint32_t* p = params;
		
		while ( *fetch != 0 )  // NULL
		{
			*p++ = (*fetch++)( *this );
			
			if ( condition != normal )
			{
				return false;
			}
		}
		
		// execute
		decoded->code( *this, params );
		
		// prefetch next
		prefetch_instruction_word();
		
		return condition == normal;
	}
	
	bool emulator::acknowledge_breakpoint( uint16_t new_opcode )
	{
		if ( !at_breakpoint() )
		{
			return false;
		}
		
		opcode = new_opcode;
		
		condition = normal;
		
		return true;
	}
	
}

