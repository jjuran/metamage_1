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
#include "v68k/op.hh"
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
	
	typedef op_result (*bkpt_handler)(processor_state& s, int vector);
	
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
		
		uint8_t* translate( addr_t a, uint32_t n, fc_t fc, mem_t access ) const
		{
			return mem.translate( a, n, fc, access );
		}
		
		bool get_byte( addr_t addr, uint8_t& x, fc_t fc ) const
		{
			return mem.get_byte( addr, x, fc );
		}
		
		bool get_word( addr_t addr, uint16_t& x, fc_t fc ) const
		{
			return mem.get_word( addr, x, fc );
		}
		
		bool get_long( addr_t addr, uint32_t& x, fc_t fc ) const
		{
			return mem.get_long( addr, x, fc );
		}
		
		bool put_byte( addr_t addr, uint8_t x, fc_t fc ) const
		{
			return mem.put_byte( addr, x, fc );
		}
		
		bool put_word( addr_t addr, uint16_t x, fc_t fc ) const
		{
			return mem.put_word( addr, x, fc );
		}
		
		bool put_long( addr_t addr, uint32_t x, fc_t fc ) const
		{
			return mem.put_long( addr, x, fc );
		}
		
		bool get_instruction_word( addr_t addr, uint16_t& x, fc_t fc ) const
		{
			return mem.get_instruction_word( addr, x, fc );
		}
		
		op_result read_byte( uint32_t addr, uint32_t& data );
		op_result read_word( uint32_t addr, uint32_t& data );
		op_result read_long( uint32_t addr, uint32_t& data );
		
		op_result read_byte_signed( uint32_t addr, uint32_t& data )
		{
			op_result result = read_byte( addr, data );
			
			data = int32_t( int8_t( data ) );
			
			return result;
		}
		
		op_result read_byte_zeroed( uint32_t addr, uint32_t& data )
		{
			op_result result = read_byte( addr, data );
			
			data = uint8_t( data );
			
			return result;
		}
		
		op_result read_word_signed( uint32_t addr, uint32_t& data )
		{
			op_result result = read_word( addr, data );
			
			data = int32_t( int16_t( data ) );
			
			return result;
		}
		
		op_result read_word_zeroed( uint32_t addr, uint32_t& data )
		{
			op_result result = read_word( addr, data );
			
			data = uint16_t( data );
			
			return result;
		}
		
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
		
		void acknowledge_breakpoint( uint16_t new_opcode )
		{
			opcode = new_opcode;
		}
	};
	
}

#endif
