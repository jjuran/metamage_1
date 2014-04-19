/*
	emulator.cc
	-----------
*/

#include "v68k/emulator.hh"

// v68k
#include "v68k/decode.hh"
#include "v68k/endian.hh"
#include "v68k/instruction.hh"
#include "v68k/load_store.hh"
#include "v68k/update_CCR.hh"


#pragma exceptions off


namespace v68k
{
	
	struct reset_vector
	{
		uint32_t isp;
		uint32_t pc;
	};
	
	
	emulator::emulator( processor_model model, const memory& mem, bkpt_handler bkpt )
	:
		processor_state( model, mem, bkpt ),
		its_instruction_counter()
	{
	}
	
	void emulator::double_bus_fault()
	{
		condition = halted;
	}
	
	void emulator::reset()
	{
		condition = normal;
		
		regs.vbr = 0;
		
		/*
			The reset vector is taken from Supervisor Program Space, but it
			*is* in fact a read operation, not execution.
		*/
		
		const uint8_t* zero = mem.translate( 0,
		                                     sizeof (reset_vector),
		                                     supervisor_program_space,
		                                     mem_read );
		
		if ( zero == 0 )  // NULL
		{
			double_bus_fault();
		}
		else
		{
			regs.ttsm = 0 << 2  // clear Trace bits
					  | 1 << 1  // set Supervisor bit
					  | 0;      // clear Master bit
			
			regs. iii = 7;  // set max Interrupt mask
			
			regs.   x = 0;  // clear CCR
			regs.nzvc = 0;
			
			const reset_vector* v = (const reset_vector*) zero;
			
			a(7)    = longword_from_big( v->isp );
			regs.pc = longword_from_big( v->pc  );
			
			// prefetch
			prefetch_instruction_word();
		}
	}
	
	bool emulator::step()
	{
		if ( condition != normal )
		{
			return false;
		}
		
	bkpt_acknowledge:
		
		// decode (prefetched)
		instruction storage = { 0 };
		
		const instruction* decoded = decode( opcode, storage );
		
		if ( !decoded )
		{
			switch ( opcode >> 12 )
			{
				case 0xA:
					return line_A_emulator();
				
				case 0xF:
					return line_F_emulator();
				
				default:
					return illegal_instruction();
			}
		}
		
		if ( (decoded->flags & not_before_mask) > model )
		{
			return illegal_instruction();
		}
		
		if ( (decoded->flags & privilege_mask) > ((regs.ttsm & 0x2) | (model == mc68000)) )
		{
			return privilege_violation();
		}
		
		const uint32_t instruction_address = regs.pc;
		
		// advance pc
		regs.pc += 2;
		
		// fetch
		fetcher* fetch = decoded->fetch;
		
		op_params pb;
		
		pb.size = decoded->size;
		
		if ( pb.size > max_actual_size )
		{
			const uint16_t size_mask = pb.size;
			
			const int bit_offset = pb.size & op_size_shift_mask;
			
			// 1 if 0 means byte-sized, 2 if 0 means word-sized
			const uint32_t index_of_zero = 1 + (pb.size & 1);
			
			pb.size = op_size_t( ((opcode & size_mask) >> bit_offset) + index_of_zero );
		}
		
		pb.target  = uint32_t( -1 );
		pb.address = regs.pc;
		
		while ( *fetch != 0 )  // NULL
		{
			(*fetch++)( *this, pb );
			
			if ( condition != normal )
			{
				return false;
			}
		}
		
		// load/store prep
		
		if ( pb.size > byte_sized  &&  badly_aligned_data( pb.address ) )
		{
			// pb.address is left set to the PC (which is always even) if unused.
			return address_error();
		}
		
		// load
		
		if ( decoded->flags & loads_and )
		{
			load( *this, pb );
			
			if ( condition != normal )
			{
				return false;
			}
		}
		
		const uint8_t saved_ttsm = regs.ttsm;
		
		// execute
		decoded->code( *this, pb );
		
		if ( at_breakpoint() )
		{
			condition = normal;
			
			goto bkpt_acknowledge;
		}
		
		// update CCR
		
		typedef instruction_flags_t flags_t;
		
		if ( const flags_t ccr_flags = flags_t( decoded->flags & CCR_update_mask ) )
		{
			if ( int32_t( pb.target ) <= 7  ||  decoded->flags & CCR_update_An )
			{
				// Don't update CCR targeting address registers unless requested
				
				const int index = ccr_flags >> CCR_update_shift;
				
				the_CCR_updaters[ index ]( *this, pb );
				
				if ( decoded->flags & CCR_update_set_X )
				{
					regs.x = regs.nzvc & 0x1;
				}
			}
		}
		
		// store
		
		if ( (decoded->flags & stores_data)  &&  !store( *this, pb ) )
		{
			return bus_error();
		}
		
		++its_instruction_counter;
		
		if ( (saved_ttsm >> 2) - 1 > 0  &&  condition >= normal )
		{
			if ( condition == stopped )
			{
				condition = normal;  // Traced STOP instructions don't stop
			}
			
			take_exception_format_2( 9 * sizeof (uint32_t), instruction_address );
		}
		
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

