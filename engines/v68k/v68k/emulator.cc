/*
	emulator.cc
	-----------
*/

#include "v68k/emulator.hh"

// v68k
#include "v68k/decode.hh"
#include "v68k/endian.hh"
#include "v68k/fetch.hh"
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
		mem( mem_base, mem_size ),
		halted( true )
	{
	}
	
	void emulator::double_bus_fault()
	{
		halted = true;
	}
	
	void emulator::reset()
	{
		const reset_vector* v;
		
		try
		{
			v = (const reset_vector*) mem.translate( 0 );
		}
		catch ( ... )
		{
			double_bus_fault();
			
			return;
		}
		
		regs.ttsm = 0 << 2  // clear Trace bits
		          | 1 << 1  // set Supervisor bit
		          | 0;      // clear Master bit
		
		regs. iii = 7;  // set max Interrupt mask
		
		regs.   x = 0;  // clear CCR
		regs.nzvc = 0;
		
		regs.a[7] = longword_from_big( v->isp );
		regs.pc   = longword_from_big( v->pc  );
		
		halted = false;
	}
	
	bool emulator::step()
	{
		if ( halted )
		{
			return false;
		}
		
		try
		{
			// fetch
			regs.op = fetch_instruction_word( regs, mem );
			
			// decode
			const instruction* decoded = decode( regs, mem );
			
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
				*p++ = (*fetch++)( regs, mem );
			}
			
			// execute
			decoded->code( regs, mem, params );
		}
		catch ( ... )
		{
			// everything halts the processor for now
			halted = true;
		}
		
		return !halted;
	}
	
}

