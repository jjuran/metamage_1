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


namespace v68k
{
	
	struct reset_vector
	{
		uint32_t isp;
		uint32_t pc;
	};
	
	
	emulator::emulator( processor_model model, uint8_t* mem_base, uint32_t mem_size )
	:
		processor_state( model, mem_base, mem_size )
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
			
			regs.a[7] = longword_from_big( v->isp );
			regs.pc   = longword_from_big( v->pc  );
			
			// prefetch
			prefetch_instruction_word();
		}
	}
	
	bool emulator::step()
	{
		if ( at_breakpoint() )
		{
			condition = normal;
			
			// Unacknowledged breakpoint traps as illegal instruction
			return illegal_instruction();
		}
		
		if ( condition != normal )
		{
			return false;
		}
		
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
		
		uint32_t* params = pb.params;
		
		uint32_t* p = params;
		
		while ( *fetch != 0 )  // NULL
		{
			*p++ = (*fetch++)( *this, pb );
			
			if ( condition != normal )
			{
				return false;
			}
		}
		
		// load/store prep
		
		const bool stores_word_in_memory = (decoded->flags & (stores_word_data|in_register)) == stores_word_data;
		
		if ( stores_word_in_memory  &&  badly_aligned_data( pb.address ) )
		{
			return address_error();
		}
		
		// load
		
		if ( (decoded->flags & loads_and)  &&  !load( *this, pb ) )
		{
			return bus_error();
		}
		
		// execute
		decoded->code( *this, pb );
		
		// store
		
		if ( !store( *this, pb, decoded->flags ) )
		{
			return bus_error();
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

