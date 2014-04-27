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
		
		regs[ VBR ] = 0;
		
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
			sr.ttsm = 0 << 2  // clear Trace bits
					| 1 << 1  // set Supervisor bit
					| 0;      // clear Master bit
			
			sr. iii = 7;  // set max Interrupt mask
			
			sr.   x = 0;  // clear CCR
			sr.nzvc = 0;
			
			const reset_vector* v = (const reset_vector*) zero;
			
			a(7) = longword_from_big( v->isp );
			pc() = longword_from_big( v->pc  );
			
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
		
		if ( (decoded->flags & privilege_mask) > ((sr.ttsm & 0x2) | (model == mc68000)) )
		{
			return privilege_violation();
		}
		
		const uint32_t instruction_address = pc();
		
		// advance pc
		pc() += 2;
		
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
		pb.address = pc();
		
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
		
		const uint8_t saved_ttsm = sr.ttsm;
		
		// execute
		const op_result result = decoded->code( *this, pb );
		
		if ( result < 0 )
		{
			switch ( result )
			{
				case Breakpoint:
					goto bkpt_acknowledge;
				
				case Illegal_instruction:
				case Trap_0:  case Trap_1:  case Trap_2:  case Trap_3:
				case Trap_4:  case Trap_5:  case Trap_6:  case Trap_7:
				case Trap_8:  case Trap_9:  case Trap_A:  case Trap_B:
				case Trap_C:  case Trap_D:  case Trap_E:  case Trap_F:
					take_exception_format_0( -result * sizeof (uint32_t) );
					
					break;
				
				case Division_by_zero:
				case CHK_exception:
				case Overflow_trap:
					take_exception_format_2( -result * sizeof (uint32_t), instruction_address );
					
					break;
				
				case Bus_error:
				case Address_error:
				case Format_error:
				default:
					condition = halted;
					
					return false;
			}
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
					sr.x = sr.nzvc & 0x1;
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
	
	bool emulator::take_exception_format_0( uint16_t vector_offset )
	{
		const uint16_t saved_sr = get_SR();
		
		set_SR( (saved_sr & 0x3FFF) | 0x2000 );  // Clear T1/T0, set S
		
		uint32_t& sp = a(7);
		
		if ( badly_aligned_data( sp ) )
		{
			return address_error();
		}
		
		const uint32_t size = 8;
		
		sp -= size;
		
		const uint32_t format_and_offset = 0 << 12 | vector_offset;
		
		const bool ok = mem.put_word( sp + 0, saved_sr,          supervisor_data_space )
		              & mem.put_long( sp + 2, pc(),              supervisor_data_space )
		              & mem.put_word( sp + 6, format_and_offset, supervisor_data_space );
		
		if ( !ok )
		{
			return bus_error();
		}
		
		if ( badly_aligned_data( regs[ VBR ] ) )
		{
			return address_error();
		}
		
		if ( !mem.get_long( regs[ VBR ] + vector_offset, pc(), supervisor_data_space ) )
		{
			return bus_error();
		}
		
		prefetch_instruction_word();
		
		return true;
	}
	
	bool emulator::take_exception_format_2( uint16_t vector_offset, uint32_t instruction_address )
	{
		const uint16_t saved_sr = get_SR();
		
		set_SR( (saved_sr & 0x3FFF) | 0x2000 );  // Clear T1/T0, set S
		
		uint32_t& sp = a(7);
		
		if ( badly_aligned_data( sp ) )
		{
			return address_error();
		}
		
		const uint32_t size = 12;
		
		sp -= size;
		
		const uint32_t format_and_offset = 2 << 12 | vector_offset;
		
		const bool ok = mem.put_word( sp + 0, saved_sr,            supervisor_data_space )
		              & mem.put_long( sp + 2, pc(),                supervisor_data_space )
		              & mem.put_word( sp + 6, format_and_offset,   supervisor_data_space )
		              & mem.put_long( sp + 8, instruction_address, supervisor_data_space );
		
		if ( !ok )
		{
			return bus_error();
		}
		
		if ( badly_aligned_data( regs[ VBR ] ) )
		{
			return address_error();
		}
		
		if ( !mem.get_long( regs[ VBR ] + vector_offset, pc(), supervisor_data_space ) )
		{
			return bus_error();
		}
		
		prefetch_instruction_word();
		
		return true;
	}
	
}
