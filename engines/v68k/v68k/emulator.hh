/*
	emulator.hh
	-----------
*/

#ifndef V68K_EMULATOR_HH
#define V68K_EMULATOR_HH

// C99
#include <stdint.h>

// v68k
#include "v68k/memory.hh"
#include "v68k/registers.hh"


namespace v68k
{
	
	enum processor_condition
	{
		startup = 0,
		
		halted = -1,
		
		normal = 1
	};
	
	class emulator
	{
		private:
			registers regs;
			
			const memory mem;
			
			processor_condition condition;
			
			void double_bus_fault();
			
		
		public:
			emulator( uint8_t* mem_base, uint32_t mem_size );
			
			const registers& get_registers() const
			{
				return regs;
			}
			
			void reset();
			
			bool step();
	};
	
}

#endif

