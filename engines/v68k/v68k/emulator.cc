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
	
	class illegal_instruction {};
	
	
	struct reset_vector
	{
		uint32_t isp;
		uint32_t pc;
	};
	
	
	emulator::emulator( uint8_t* mem_base, uint32_t mem_size )
	:
		processor_state( mem_base, mem_size )
	{
	}
	
	void emulator::prefetch_instruction_word()
	{
		regs.op = mem.get_instruction_word( regs.pc );
	}
	
	void emulator::double_bus_fault()
	{
		condition = halted;
	}
	
	void emulator::reset()
	{
		try
		{
			const reset_vector* v = (const reset_vector*) mem.translate( 0 );
			
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
		
		condition = normal;
	}
	
	bool emulator::step()
	{
		if ( condition != normal )
		{
			return false;
		}
		
		try
		{
			// advance pc
			regs.pc += 2;
			
			// decode (prefetched)
			const instruction* decoded = decode( regs.op );
			
			if ( !decoded )
			{
				throw illegal_instruction();
			}
			
			// prepare
			fetcher* fetch = decoded->fetch;
			
			uint32_t params[ max_params ];
			
			uint32_t* p = params;
			
			while ( *fetch != 0 )  // NULL
			{
				*p++ = (*fetch++)( *this );
			}
			
			// execute
			decoded->code( *this, params );
			
			// prefetch next
			prefetch_instruction_word();
		}
		catch ( ... )
		{
			// everything halts the processor for now
			condition = halted;
		}
		
		return condition == normal;
	}
	
}

