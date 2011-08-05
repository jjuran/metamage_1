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
			void double_bus_fault();
		
		public:
			emulator( processor_model model, uint8_t* mem_base, uint32_t mem_size );
			
			void reset();
			
			bool step();
			
			bool at_breakpoint() const
			{
				return (condition & bkpt_mask) == bkpt_0;
			}
			
			bool acknowledge_breakpoint( uint16_t new_opcode );
			
			bool illegal_instruction();
			bool privilege_violation();
	};
	
}

#endif

