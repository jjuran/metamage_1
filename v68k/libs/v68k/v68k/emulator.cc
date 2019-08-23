/*
	emulator.cc
	-----------
*/

#include "v68k/emulator.hh"

// v68k
#include "v68k/decode.hh"
#include "v68k/instruction.hh"
#include "v68k/load_store.hh"
#include "v68k/update_CCR.hh"


#ifdef __MWERKS__
#pragma exceptions off
#endif


namespace v68k
{
	
	emulator::emulator( processor_model model, const memory& mem, bkpt_handler bkpt )
	:
		processor_state( model, mem, bkpt ),
		its_instruction_counter()
	{
	}
	
	void emulator::reset()
	{
		condition = normal;
		
		regs[ VBR ] = 0;
		
		/*
			The reset vector is taken from Supervisor Program Space, but it
			*is* in fact a read operation, not execution.
		*/
		
		const bool ok = mem.get_long( 0, a(7), supervisor_program_space )  &&
		                mem.get_long( 4, pc(), supervisor_program_space );
		
		if ( ! ok )
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
			
			// prefetch
			prefetch_instruction_word();
		}
	}
	
	bool emulator::step()
	{
	bkpt_acknowledge:
		
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
			const op_result result = (*fetch++)( *this, pb );
			
			if ( result < 0 )
			{
				return fault( result, instruction_address );
			}
			
			if ( condition != normal )
			{
				return false;
			}
		}
		
		// load/store prep
		
		if (
			+ pb.size > byte_sized              &&
			+ badly_aligned_data( pb.address )  &&
			+ !decoded->accesses_bytes_only()   &&
			+ true )
		{
			// pb.address is left set to the PC (which is always even) if unused.
			return fault( Address_error, instruction_address );
		}
		
		// load
		
		if ( decoded->flags & loads_and )
		{
			const op_result result = load( *this, pb );
			
			if ( result < 0 )
			{
				return fault( result, instruction_address );
			}
		}
		
		// stack alignment check
		
		if ( decoded->flags & uses_stack )
		{
			if ( badly_aligned_data( a(7) ) )
			{
				return address_error();
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
					take_exception( 0, -result * sizeof (uint32_t) );
					
					break;
				
				case Bus_error:
				case Address_error:
				case Format_error:
				
				case Division_by_zero:
				case CHK_exception:
				case Overflow_trap:
					fault( result, instruction_address );
					
					break;
				
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
			return fault( Bus_error, instruction_address );
		}
		
		++its_instruction_counter;
		
		if ( (saved_ttsm >> 2) - 1 > 0  &&  condition >= normal )
		{
			if ( condition == stopped )
			{
				condition = normal;  // Traced STOP instructions don't stop
			}
			
			fault( Trace_exception, instruction_address );
		}
		
		// prefetch next
		prefetch_instruction_word();
		
		return condition == normal;
	}
	
	void emulator::prefetch_instruction_word()
	{
		if ( pc() & 1 )
		{
			address_error();
		}
		else if ( !mem.get_instruction_word( pc(), opcode, program_space() ) )
		{
			bus_error();
		}
	}
	
	bool emulator::take_exception( uint16_t  format,
	                               uint16_t  vector_offset,
	                               uint32_t  instruction_address )
	{
		const uint16_t saved_sr = get_SR();
		
		set_SR( (saved_sr & 0x3FFF) | 0x2000 );  // Clear T1/T0, set S
		
		uint32_t& sp = a(7);
		
		if ( badly_aligned_data( sp ) )
		{
			return address_error();
		}
		
		const uint32_t size = format <= 1 ? 8 : 12;
		
		sp -= size;
		
		const uint16_t format_and_offset = format << 12 | vector_offset;
		
		bool ok = mem.put_word( sp + 0, saved_sr,            supervisor_data_space )
		        & mem.put_long( sp + 2, pc(),                supervisor_data_space )
		        & mem.put_word( sp + 6, format_and_offset,   supervisor_data_space );
		
		if ( ok  &&  format == 2 )
		{
			ok = mem.put_long( sp + 8, instruction_address, supervisor_data_space );
		}
		
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
	
	bool emulator::interrupt( int level, int vector )
	{
	//	ASSERT( level >= 0 );
	//	ASSERT( level <= 7 );
	
	//	ASSERT( vector == 0  ||  vector >= 64 );
	//	ASSERT( vector < 256 );
		
		if ( level > 7  ||  vector >= 256 )
		{
			return false;
		}
		
		if ( level <= sr.iii  &&  level != 7 )
		{
			return false;
		}
		
		if ( vector == 0 )
		{
			vector = 24 + level;  // autovector
		}
		else if ( vector < 64 )
		{
			return false;
		}
		
		if ( condition == stopped )
		{
			condition = normal;
		}
		
		const uint16_t vector_offset = vector * sizeof (uint32_t);
		
		if ( bool ok = take_exception( 0, vector_offset ) )
		{
			sr.iii = level;
			
			if ( sr.ttsm & 0x1 )
			{
				sr.ttsm &= ~0x1;  // Clear M
				
				ok = take_exception( 1, vector_offset );
			}
			
			return ok;
		}
		
		return false;
	}
	
}
