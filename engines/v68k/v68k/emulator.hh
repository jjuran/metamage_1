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
			unsigned long its_instruction_counter;
			
			void double_bus_fault();
		
		public:
			emulator( processor_model model, const memory& mem );
			
			unsigned long instruction_count() const  { return its_instruction_counter; }
			
			void reset();
			
			bool step();
			
			bool at_breakpoint() const
			{
				return (condition & bkpt_mask) == bkpt_0;
			}
			
			bool acknowledge_breakpoint( uint16_t new_opcode );
			
			bool illegal_instruction()  { return take_exception_format_0( 4 * sizeof (uint32_t) ); }
			bool privilege_violation()  { return take_exception_format_0( 8 * sizeof (uint32_t) ); }
			
			bool line_A_emulator()  { return take_exception_format_0( 10 * sizeof (uint32_t) ); }
			bool line_F_emulator()  { return take_exception_format_0( 11 * sizeof (uint32_t) ); }
	};
	
}

#endif

