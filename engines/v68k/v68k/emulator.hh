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
	
	class emulator
	{
		private:
			registers regs;
			
			const memory mem;
			
			bool halted;
			
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

