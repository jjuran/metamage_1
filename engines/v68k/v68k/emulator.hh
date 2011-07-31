/*
	emulator.hh
	-----------
*/

#ifndef V68K_EMULATOR_HH
#define V68K_EMULATOR_HH

// C99
#include <stdint.h>

// v68k
#include "v68k/state.hh"


namespace v68k
{
	
	class emulator : public processor_state
	{
		private:
			void prefetch_instruction_word();
			
			void double_bus_fault();
		
		public:
			emulator( processor_model model, uint8_t* mem_base, uint32_t mem_size );
			
			void reset();
			
			bool step();
	};
	
}

#endif

