/*
	state.hh
	--------
*/

#ifndef V68K_STATE_HH
#define V68K_STATE_HH

// C99
#include <stdint.h>

// v68k
#include "v68k/memory.hh"
#include "v68k/op_params.hh"
#include "v68k/registers.hh"


namespace v68k
{
	
	enum processor_model
	{
		mc68000 = 0x00,
		mc68010 = 0x10,
		mc68020 = 0x20,
		mc68030 = 0x30,
		mc68040 = 0x40
	};
	
	enum processor_condition
	{
		startup = 0,
		
		halted  = -1,  // double bus fault
		
		finished = 3,  // STOP #FFFF (v68k extension)
		stopped  = 2,  // STOP instruction
		
		normal = 1
	};
	
	struct processor_state;
	
	typedef uint16_t (*bkpt_handler)(processor_state& s, int vector);
	
	struct processor_state
	{
		uint32_t regs[ n_registers ];
		
		status_register sr;
		
		const memory& mem;
		
		const bkpt_handler bkpt;
		
		const processor_model model;
		
		processor_condition condition;
		
		uint16_t opcode;  // current instruction opcode
		
		processor_state( processor_model model, const memory& mem, bkpt_handler bkpt );
		
		uint32_t& d( int i )  { return regs[ D0 + i ]; }
		uint32_t& a( int i )  { return regs[ A0 + i ]; }
		
		const uint32_t& d( int i ) const  { return regs[ D0 + i ]; }
		const uint32_t& a( int i ) const  { return regs[ A0 + i ]; }
		
		uint32_t& saved_sp()
		{
			return + !(sr.ttsm & 0x2) ? regs[ USP ]
			       : !(sr.ttsm & 0x1) ? regs[ ISP ]
			       :                    regs[ MSP ];
		}
		
		void save_sp()
		{
			saved_sp() = a(7);
		}
		
		void load_sp()
		{
			a(7) = saved_sp();
		}
		
		uint32_t& pc()
		{
			return regs[ PC ];
		}
		
		uint32_t read_mem( uint32_t addr, op_size_t size );
		
		uint16_t get_CCR() const;
		
		uint16_t get_SR() const;
		
		void set_CCR( uint16_t new_sr );
		
		void set_SR( uint16_t new_sr );
		
		function_code_t data_space() const
		{
			return sr.ttsm & 0x2 ? supervisor_data_space
			                     : user_data_space;
		}
		
		function_code_t program_space() const
		{
			return sr.ttsm & 0x2 ? supervisor_program_space
			                     : user_program_space;
		}
		
		bool badly_aligned_data( uint32_t addr ) const
		{
			/*
				Return true for odd addresses, but only on the 68000/68010.
				The inequality evaluates to 1 or 0 for masking the address.
			*/
			
			return (model < mc68020) & addr;
		}
		
		uint32_t bus_error    ()  { condition = halted;  return 0; }
		uint32_t address_error()  { condition = halted;  return 0; }
	};
	
}

#endif

