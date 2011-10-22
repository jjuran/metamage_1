/*
	microcode.cc
	------------
*/

#include "v68k/microcode.hh"

// v68k
#include "v68k/conditional.hh"
#include "v68k/state.hh"


namespace v68k
{
	
	#define N( x )  (!!(x) << 3)
	#define Z( x )  (!!(x) << 2)
	#define V( x )  (!!(x) << 1)
	#define C( x )  (!!(x) << 0)
	
	
	#pragma mark Line 0
	
	void microcode_ORI_to_CCR( processor_state& s, op_params& pb )
	{
		const uint32_t data = pb.params[0];
		
		s.set_CCR( s.get_CCR() | data );
	}
	
	void microcode_ORI_to_SR( processor_state& s, op_params& pb )
	{
		const uint32_t data = pb.params[0];
		
		s.set_SR( s.get_SR() | data );
	}
	
	void microcode_BCHG( processor_state& s, op_params& pb )
	{
		microcode_BTST( s, pb );
		
		pb.params[1] ^= 1 << pb.params[0];
	}
	
	void microcode_BCLR( processor_state& s, op_params& pb )
	{
		microcode_BTST( s, pb );
		
		pb.params[1] &= ~(1 << pb.params[0]);
	}
	
	void microcode_BSET( processor_state& s, op_params& pb )
	{
		microcode_BTST( s, pb );
		
		pb.params[1] |= 1 << pb.params[0];
	}
	
	void microcode_BTST( processor_state& s, op_params& pb )
	{
		const uint32_t bit = pb.params[0];
		
		s.regs.nzvc &= ~0x4;
		s.regs.nzvc |= (~pb.params[1] >> bit & 0x1) << 2;
	}
	
	void microcode_MOVEP_to( processor_state& s, op_params& pb )
	{
		const uint32_t mode = pb.params[0];
		const uint32_t x    = pb.params[1];
		const uint32_t y    = pb.params[2];
		const int32_t  disp = pb.params[3];
		
		const uint32_t Dx = s.regs.d[ x ];
		const uint32_t Ay = s.regs.a[ y ];
		
		const bool doubled = mode & 0x1;
		
		const uint32_t addr = Ay + disp;
		
		uint8_t* p = s.mem.translate( addr, (4 << doubled) - 1, s.data_space(), mem_write );
		
		if ( p == 0 )  // NULL
		{
			s.bus_error();
			
			return;
		}
		
		switch ( doubled )
		{
			case true:
				p[0] = Dx >> 24;
				p[2] = Dx >> 16;
				
				p += 4;
				
				// fall through
			
			case false:
				p[0] = Dx >>  8;
				p[2] = Dx >>  0;
		}
	}
	
	void microcode_MOVEP_from( processor_state& s, op_params& pb )
	{
		const uint32_t mode = pb.params[0];
		const uint32_t x    = pb.params[1];
		const uint32_t y    = pb.params[2];
		const int32_t  disp = pb.params[3];
		
		uint32_t&      Dx = s.regs.d[ x ];
		uint32_t const Ay = s.regs.a[ y ];
		
		const bool doubled = mode & 0x1;
		
		const uint32_t addr = Ay + disp;
		
		const uint8_t* p = s.mem.translate( addr, (4 << doubled) - 1, s.data_space(), mem_read );
		
		if ( p == 0 )  // NULL
		{
			s.bus_error();
			
			return;
		}
		
		uint32_t data = doubled ? 0 : Dx & 0xFFFF0000;
		
		switch ( doubled )
		{
			case true:
				data |= p[0] << 24;
				data |= p[2] << 16;
				
				p += 4;
				
				// fall through
			
			case false:
				data |= p[0] << 8;
				data |= p[2] << 0;
		}
		
		Dx = data;
	}
	
	void microcode_ANDI_to_CCR( processor_state& s, op_params& pb )
	{
		const uint32_t data = pb.params[0];
		
		s.set_CCR( s.get_CCR() & data );
	}
	
	void microcode_ANDI_to_SR( processor_state& s, op_params& pb )
	{
		const uint32_t data = pb.params[0];
		
		s.set_SR( s.get_SR() & data );
	}
	
	void microcode_EORI_to_CCR( processor_state& s, op_params& pb )
	{
		const uint32_t data = pb.params[0];
		
		s.set_CCR( s.get_CCR() ^ data );
	}
	
	void microcode_EORI_to_SR( processor_state& s, op_params& pb )
	{
		const uint32_t data = pb.params[0];
		
		s.set_SR( s.get_SR() ^ data );
	}
	
	void microcode_MOVES( processor_state& s, op_params& pb )
	{
		const uint32_t addr = pb.params[1];
		const uint32_t more = pb.params[2];
		
		const int size = 1 << pb.size - 1;  // 1,2,4
		
		const uint16_t reg_id = more >> 12;
		
		const uint16_t writing = more & 0x0800;
		
		const function_code_t fc = function_code_t( writing ? s.regs.dfc : s.regs.sfc );
		
		const memory_access_t access = writing ? mem_write : mem_read;
		
		uint8_t* p = s.mem.translate( addr, size, fc, access );
		
		if ( p == 0 )
		{
			s.bus_error();
			
			return;
		}
		
		if ( writing )
		{
			const uint32_t data = s.regs.d[ reg_id ];
			
			switch ( pb.size )
			{
				case long_sized:
					*p++ = data >> 24;
					*p++ = data >> 16 & 0xFF;
				case word_sized:
					*p++ = data >>  8 & 0xFF;
				case byte_sized:
					*p++ = data >>  0 & 0xFF;
			}
		}
		else
		{
			uint32_t& reg = s.regs.d[ reg_id ];
			
			switch ( pb.size )
			{
				case long_sized:
					reg = p[0] << 24
					    | p[1] << 16
					    | p[2] <<  8
					    | p[3] <<  0;
					
					break;
				
				case word_sized:
					{
						const uint16_t data = p[0] << 8 | p[1];
						
						if ( reg_id & 0x8 )
						{
							reg = int32_t( int16_t( data ) );
						}
						else
						{
							reg &= 0xFFFF0000;
							
							reg |= data;
						}
					}
					
					break;
				
				case byte_sized:
					if ( reg_id & 0x8 )
					{
						reg = int32_t( int8_t( p[0] ) );
					}
					else
					{
						reg &= 0xFFFFFF00;
						
						reg |= p[0];
					}
					
					break;
			}
		}
	}
	
	#pragma mark -
	#pragma mark Lines 1-3
	
	void microcode_MOVE( processor_state& s, op_params& pb )
	{
		const uint32_t data = pb.params[0];
		
		pb.params[1] = data;
	}
	
	#pragma mark -
	#pragma mark Line 4
	
	void microcode_LEA( processor_state& s, op_params& pb )
	{
		const uint32_t addr = pb.params[0];
		const uint32_t n    = pb.params[1];
		
		uint32_t& An = s.regs.a[n];
		
		An = addr;
	}
	
	void microcode_NEG( processor_state& s, op_params& pb )
	{
		pb.params[0] = pb.params[1];
		pb.params[1] = 0;
		
		microcode_SUB( s, pb );
		
		const uint8_t C = ~s.regs.nzvc >> 2 & 0x1;  // Set C to !Z
		
		s.regs.nzvc = s.regs.nzvc & ~0x1 | C;
		
		s.regs.x = C;
		
	}
	
	void microcode_MOVE_from_CCR( processor_state& s, op_params& pb )
	{
		pb.params[1] = s.get_CCR();
	}
	
	void microcode_MOVE_from_SR( processor_state& s, op_params& pb )
	{
		pb.params[1] = s.get_SR();
	}
	
	void microcode_MOVE_to_CCR( processor_state& s, op_params& pb )
	{
		const uint32_t data = pb.params[0];
		
		s.set_CCR( data );
	}
	
	void microcode_MOVE_to_SR( processor_state& s, op_params& pb )
	{
		const uint32_t data = pb.params[0];
		
		s.set_SR( data );
	}
	
	void microcode_SWAP( processor_state& s, op_params& pb )
	{
		const uint32_t n = pb.params[0];
		
		const uint32_t data = s.regs.d[n];
		
		s.regs.d[n] = data << 16
		            | data >> 16;
	}
	
	void microcode_BKPT( processor_state& s, op_params& pb )
	{
		const uint32_t data = pb.params[0];  // 3-bit breakpoint vector
		
		s.regs.pc -= 2;
		
		s.condition = processor_condition( bkpt_0 + data );
	}
	
	void microcode_PEA( processor_state& s, op_params& pb )
	{
		const uint32_t addr = pb.params[0];
		
		uint32_t& sp = s.regs.a[7];
		
		if ( s.badly_aligned_data( sp ) )
		{
			s.address_error();
			
			return;
		}
		
		sp -= 4;
		
		if ( !s.mem.put_long( sp, addr, s.data_space() ) )
		{
			s.bus_error();
		}
	}
	
	void microcode_EXT_W( processor_state& s, op_params& pb )
	{
		const uint32_t n = pb.params[0];
		
		uint32_t& Dn = s.regs.d[n];
		
		const int8_t byte = Dn;
		
		const int16_t word = byte;
		
		Dn = (Dn & 0xFFFF0000) | word;
		
		s.regs.nzvc = N( word <  0 )
		            | Z( word == 0 )
		            | V( 0 )
		            | C( 0 );
	}
	
	void microcode_EXT_L( processor_state& s, op_params& pb )
	{
		const uint32_t n = pb.params[0];
		
		uint32_t& Dn = s.regs.d[n];
		
		const int16_t word = Dn;
		
		const int32_t longword = word;
		
		Dn = longword;
		
		s.regs.nzvc = N( longword <  0 )
		            | Z( longword == 0 )
		            | V( 0 )
		            | C( 0 );
	}
	
	void microcode_EXTB( processor_state& s, op_params& pb )
	{
		const uint32_t n = pb.params[0];
		
		uint32_t& Dn = s.regs.d[n];
		
		const int8_t byte = Dn;
		
		const int32_t longword = byte;
		
		Dn = longword;
		
		s.regs.nzvc = N( longword <  0 )
		            | Z( longword == 0 )
		            | V( 0 )
		            | C( 0 );
	}
	
	void microcode_MOVEM_to( processor_state& s, op_params& pb )
	{
		uint32_t addr = pb.params[1];
		uint16_t mask = pb.params[3];
		
		const uint16_t movem_size_code = pb.params[0];
		const uint16_t update_register = pb.params[2];
		
		const uint16_t longword_sized = movem_size_code & 0x1;
		
		int32_t increment = 2 << longword_sized;
		
		if ( update_register )
		{
			increment = -increment;
		}
		
		for ( int r = 0;  mask != 0;  ++r, mask >>= 1 )
		{
			if ( mask & 0x1 )
			{
				const uint32_t data = s.regs.d[ update_register ? 15 - r : r ];
				
				const bool ok = longword_sized ? s.mem.put_long( addr, data, s.data_space() )
											   : s.mem.put_word( addr, data, s.data_space() );
				
				if ( !ok )
				{
					s.bus_error();
					
					return;
				}
				
				addr += increment;
			}
		}
		
		if ( update_register )
		{
			s.regs.d[ update_register ] = addr - increment;
		}
	}
	
	void microcode_MOVEM_from( processor_state& s, op_params& pb )
	{
		uint32_t addr = pb.params[1];
		uint16_t mask = pb.params[3];
		
		const uint16_t movem_size_code = pb.params[0];
		const uint16_t update_register = pb.params[2];
		
		const uint16_t longword_sized = movem_size_code & 0x1;
		
		const int32_t increment = 2 << longword_sized;
		
		for ( int r = 0;  mask != 0;  ++r, mask >>= 1 )
		{
			if ( mask & 0x1 )
			{
				uint32_t data;
				
				bool ok;
				
				if ( longword_sized )
				{
					ok = s.mem.get_long( addr, data, s.data_space() );
				}
				else
				{
					uint16_t word;
					
					ok = s.mem.get_word( addr, word, s.data_space() );
					
					data = int32_t( int16_t( word ) );
				}
				
				if ( !ok )
				{
					s.bus_error();
					
					return;
				}
				
				s.regs.d[r] = data;
				
				addr += increment;
			}
		}
		
		if ( update_register )
		{
			s.regs.d[ update_register ] = addr;
		}
	}
	
	void microcode_TRAP( processor_state& s, op_params& pb )
	{
		const uint32_t trap_number = pb.params[0];
		
		s.take_exception_format_0( (trap_number + 32) * sizeof (uint32_t) );
	}
	
	void microcode_LINK( processor_state& s, op_params& pb )
	{
		const uint32_t n    = pb.params[0];
		const int32_t  disp = pb.params[1];
		
		uint32_t& An = s.regs.a[n];
		uint32_t& sp = s.regs.a[7];
		
		if ( s.badly_aligned_data( sp ) )
		{
			s.address_error();
			
			return;
		}
		
		sp -= 4;
		
		if ( !s.mem.put_long( sp, An, s.data_space() ) )
		{
			s.bus_error();
			
			return;
		}
		
		An = sp;
		
		sp += disp;
	}
	
	void microcode_UNLK( processor_state& s, op_params& pb )
	{
		const uint32_t n = pb.params[0];
		
		uint32_t& An = s.regs.a[n];
		uint32_t& sp = s.regs.a[7];
		
		sp = An;
		
		if ( s.badly_aligned_data( sp ) )
		{
			s.address_error();
			
			return;
		}
		
		if ( !s.mem.get_long( sp, An, s.data_space() ) )
		{
			s.bus_error();
			
			return;
		}
		
		sp += 4;
	}
	
	void microcode_MOVE_to_USP( processor_state& s, op_params& pb )
	{
		const uint32_t n = pb.params[0];
		
		// MOVE USP is privileged, so USP is never A7 here
		s.regs.alt_sp = s.regs.a[n];
	}
	
	void microcode_MOVE_from_USP( processor_state& s, op_params& pb )
	{
		const uint32_t n = pb.params[0];
		
		// MOVE USP is privileged, so USP is never A7 here
		s.regs.a[n] = s.regs.alt_sp;
	}
	
	void microcode_NOP( processor_state& s, op_params& pb )
	{
		// "no operation"
	}
	
	void microcode_STOP( processor_state& s, op_params& pb )
	{
		const uint32_t data = pb.params[0];
		
		if ( data == 0xFFFF )
		{
			s.condition = finished;
		}
		else
		{
			s.set_SR( data );
			
			s.condition = stopped;
		}
	}
	
	void microcode_RTE( processor_state& s, op_params& pb )
	{
		uint32_t& sp = s.regs.a[7];
		
		if ( s.badly_aligned_data( sp ) )
		{
			s.address_error();
			
			return;
		}
		
		uint16_t id;
		
		if ( !s.mem.get_word( sp + 6, id, s.data_space() ) )
		{
			s.bus_error();
			
			return;
		}
		
		const uint16_t format = id >> 12;
		
		if ( format != 0 )
		{
			s.format_error();
			
			return;
		}
		
		if ( !s.mem.get_long( sp + 2, s.regs.pc, s.data_space() ) )
		{
			s.bus_error();
			
			return;
		}
		
		uint16_t saved_sr;
		
		if ( !s.mem.get_word( sp, saved_sr, s.data_space() ) )
		{
			s.bus_error();
			
			return;
		}
		
		sp += 8;  // format 0
		
		s.set_SR( saved_sr );
	}
	
	void microcode_RTD( processor_state& s, op_params& pb )
	{
		microcode_RTS( s, pb );
		
		s.regs.a[7] += pb.params[0];
	}
	
	void microcode_RTS( processor_state& s, op_params& pb )
	{
		uint32_t& sp = s.regs.a[7];
		
		if ( s.badly_aligned_data( sp ) )
		{
			s.address_error();
			
			return;
		}
		
		if ( !s.mem.get_long( sp, s.regs.pc, s.data_space() ) )
		{
			s.bus_error();
			
			return;
		}
		
		sp += 4;
	}
	
	void microcode_TRAPV( processor_state& s, op_params& pb )
	{
		if ( s.get_CCR() & 0x2 )
		{
			s.take_exception_format_6( 7 * sizeof (uint32_t), s.regs.pc - 2 );
		}
	}
	
	void microcode_RTR( processor_state& s, op_params& pb )
	{
		uint32_t& sp = s.regs.a[7];
		
		if ( s.badly_aligned_data( sp ) )
		{
			s.address_error();
			
			return;
		}
		
		uint16_t ccr;
		
		if ( !s.mem.get_word( sp, ccr, s.data_space() ) )
		{
			s.bus_error();
			
			return;
		}
		
		s.set_CCR( ccr );
		
		sp += 2;
		
		if ( !s.mem.get_long( sp, s.regs.pc, s.data_space() ) )
		{
			s.bus_error();
			
			return;
		}
		
		sp += 4;
	}
	
	enum
	{
		only_3_bits = 0x1
	};
	
	static const uint16_t control_register_flags[] =
	{
		0x01,  // SFC, 3 bits
		0x01,  // DFC, 3 bits
		0,     // CACR
		0,     // TC
		0,     // ITT0
		0,     // ITT1
		0,     // DTT0
		0,     // DTT1
		
		0x00,  // USP
		0x00,  // VBR
		0,     // CAAR
		0x20,  // MSP, 68020+
		0x20,  // ISP, 68020+
		0,     // MMUSR
		0,     // URP
		0      // SRP
	};
	
	static inline uint16_t index_of_control_register( uint16_t id )
	{
		return id >> 8 | id & ~0x0800;
	}
	
	static uint32_t* get_control_register( processor_state& s, uint16_t control_index )
	{
		switch ( control_index )
		{
			case 0x0:  // 0x000
				return &s.regs.sfc;
			
			case 0x1:  // 0x001
				return &s.regs.dfc;
			
			case 0x8:  // 0x800
				// MOVEC is privileged, so alt SP is always USP
				return &s.regs.alt_sp;
			
			case 0x9:  // 0x801
				return &s.regs.vbr;
			
			case 0xB:  // 0x803, MSP
			case 0xC:  // 0x804, ISP
				// MOVEC is privileged, so A7 is always SSP
				return (s.regs.ttsm ^ control_index) & 0x1 ? &s.regs.alt_ssp
				                                           : &s.regs.a[7];
			
			default:
				break;
		}
		
		return 0;  // NULL
	}
	
	void microcode_MOVEC( processor_state& s, op_params& pb )
	{
		const uint32_t writing   = pb.params[0];
		const uint32_t registers = pb.params[1];
		
		const uint16_t general_id = registers >> 12;
		const uint16_t control_id = registers & 0x0FFF;
		
		const uint32_t* control_register = 0;  // NULL
		
		const uint16_t control_index = index_of_control_register( control_id );
		
		if ( control_index & ~0x000F )
		{
			// control index is out of range; ergo, control id is invalid
		}
		else if ( const uint32_t* control_register = get_control_register( s, control_index ) )
		{
			const uint16_t flags = control_register_flags[ control_index ];
			
			if ( s.model < flags )
			{
				// This control register doesn't exist on this processor model
			}
			else
			{
				if ( writing )
				{
					uint32_t data = s.regs.d[ general_id ];
					
					if ( flags & only_3_bits )
					{
						data &= 0x7;
					}
				}
				else
				{
					s.regs.d[ general_id ] = *control_register;
				}
				
				return;  // Success
			}
		}
		
		// Failure
		
		s.regs.pc -= 4;
		
		s.take_exception_format_0( 4 * sizeof (uint32_t) );  // Illegal Instruction
	}
	
	void microcode_JSR( processor_state& s, op_params& pb )
	{
		const uint32_t addr = pb.params[0];
		
		uint32_t& sp = s.regs.a[7];
		
		if ( s.badly_aligned_data( sp ) )
		{
			s.address_error();
			
			return;
		}
		
		sp -= 4;
		
		if ( !s.mem.put_long( sp, s.regs.pc, s.data_space() ) )
		{
			s.bus_error();
			
			return;
		}
		
		s.regs.pc = addr;
	}
	
	void microcode_JMP( processor_state& s, op_params& pb )
	{
		const uint32_t addr = pb.params[0];
		
		s.regs.pc = addr;
	}
	
	#pragma mark -
	#pragma mark Line 5
	
	void microcode_Scc( processor_state& s, op_params& pb )
	{
		const uint16_t cc = pb.params[0];
		
		pb.params[1] = int32_t() - test_conditional( cc, s.regs.nzvc );
	}
	
	#pragma mark -
	#pragma mark Line 6
	
	void microcode_BRA( processor_state& s, op_params& pb )
	{
		const uint32_t pc   = pb.params[0];
		const int32_t  disp = pb.params[1];
		
		s.regs.pc = pc + disp;
	}
	
	void microcode_BSR( processor_state& s, op_params& pb )
	{
		const uint32_t pc   = pb.params[0];
		const int32_t  disp = pb.params[1];
		
		uint32_t& sp = s.regs.a[7];
		
		if ( s.badly_aligned_data( sp ) )
		{
			s.address_error();
			
			return;
		}
		
		sp -= 4;
		
		if ( !s.mem.put_long( sp, s.regs.pc, s.data_space() ) )
		{
			s.bus_error();
			
			return;
		}
		
		s.regs.pc = pc + disp;
	}
	
	void microcode_Bcc( processor_state& s, op_params& pb )
	{
		const uint32_t pc   = pb.params[0];
		const int32_t  disp = pb.params[1];
		const uint32_t cc   = pb.params[2];
		
		if ( test_conditional( cc, s.regs.nzvc ) )
		{
			s.regs.pc = pc + disp;
		}
	}
	
	#pragma mark -
	#pragma mark Line 8
	
	void microcode_OR( processor_state& s, op_params& pb )
	{
		const uint32_t data = pb.params[0];
		
		pb.params[1] |= data;
	}
	
	#pragma mark -
	#pragma mark Line 9
	
	static int32_t subtract( processor_state& s, int32_t data, int32_t from )
	{
		const int32_t diff = from - data;
		
		const bool S = data < 0;
		const bool D = from < 0;
		const bool R = diff < 0;
		
		s.regs.nzvc = N( diff <  0 )
		            | Z( diff == 0 )
		            | V( (S == R) & (S != D) )
		            | C( S & !D | R & !D | S & R );
		
		return diff;
	}
	
	void microcode_SUB( processor_state& s, op_params& pb )
	{
		const int32_t a = pb.params[0];
		const int32_t b = pb.params[1];
		
		const int32_t diff = subtract( s, a, b );
		
		pb.params[1] = diff;
		
		s.regs.x = s.regs.nzvc & 0x1;
	}
	
	void microcode_SUBA( processor_state& s, op_params& pb )
	{
		const int32_t a = pb.params[0];
		const int32_t b = pb.params[1];
		
		const int32_t diff = b - a;
		
		pb.params[1] = diff;
	}
	
	#pragma mark -
	#pragma mark Line B
	
	void microcode_CMP( processor_state& s, op_params& pb )
	{
		const int32_t a = pb.params[0];
		const int32_t b = pb.params[1];
		
		(void) subtract( s, a, b );
	}
	
	void microcode_EOR( processor_state& s, op_params& pb )
	{
		const uint32_t data = pb.params[0];
		
		pb.params[1] ^= data;
	}
	
	#pragma mark -
	#pragma mark Line C
	
	void microcode_AND( processor_state& s, op_params& pb )
	{
		const uint32_t data = pb.params[0];
		
		pb.params[1] &= data;
	}
	
	void microcode_EXG( processor_state& s, op_params& pb )
	{
		const uint32_t mode = pb.params[0];
		const uint32_t x    = pb.params[1];  // 3-bit register number
		const uint32_t y    = pb.params[2];  // 4-bit register id
		
		const uint32_t dA = (mode << 3) & mode;  // 0 for D or 8 for A
		
		uint32_t& Rx = s.regs.d[ dA + x ];
		uint32_t& Ry = s.regs.d[      y ];
		
		uint32_t temp = Rx;
		
		Rx = Ry;
		
		Ry = temp;
	}
	
	#pragma mark -
	#pragma mark Line D
	
	void microcode_ADD( processor_state& s, op_params& pb )
	{
		const int32_t a = pb.params[0];
		const int32_t b = pb.params[1];
		
		/*
			     size_code  E  (0, 1,  2)
			1 << size_code  E  (1, 2,  4)
			      
			             n_bytes - 1   E  (0, 1,  3)
			        8 * (n_bytes - 1)  E  (0, 8, 24)
			
			0x80 << 8 * (n_bytes - 1)  E  (0x80, 0x8000, 0x80000000)
		*/
		
		const int n_bytes = 1 << pb.size - 1;
		
		const uint32_t sign_mask = 0x80 << 8 * (n_bytes - 1);
		
		const int32_t sum = a + b;
		
		const bool S = a   & sign_mask;
		const bool D = b   & sign_mask;
		const bool R = sum & sign_mask;
		
		s.regs.nzvc = N( R )
		            | Z( sum == 0 )
		            | V( (S == D) & (S != R) )
		            | C( S & D | !R & D | S & !R );
		
		s.regs.x = s.regs.nzvc & 0x1;
		
		pb.params[1] = sum;
	}
	
	void microcode_ADDA( processor_state& s, op_params& pb )
	{
		const int32_t a = pb.params[0];
		const int32_t b = pb.params[1];
		
		const int32_t sum = a + b;
		
		pb.params[1] = sum;
	}
	
	void microcode_ASR( processor_state& s, op_params& pb )
	{
		int32_t data = pb.params[1];
		
		const uint16_t count = pb.params[0];
		
		/*
			     size_code  E  (0, 1,  2)
			1 << size_code  E  (1, 2,  4)
			      
			             n_bytes - 1   E  (0, 1,  3)
			        8 * (n_bytes - 1)  E  (0, 8, 24)
			
			0x80 << 8 * (n_bytes - 1)  E  (0x80, 0x8000, 0x80000000)
		*/
		
		const int n_bytes = 1 << pb.size - 1;
		
		const uint32_t sign_mask = 0x80 << 8 * (n_bytes - 1);
		const uint32_t data_mask = (sign_mask << 1) - 1;
		
		bool last_bit = 0;
		
		for ( int i = count;  i > 0;  --i )
		{
			last_bit = data & 0x1;
			
			s.regs.x = last_bit;
			
			data >>= 1;
		}
		
		s.regs.nzvc = N( data & sign_mask )
		            | Z( (data & data_mask) == 0 )
		            | V( 0 )
		            | C( last_bit );
		
		pb.params[1] = data;
	}
	
	void microcode_ASL( processor_state& s, op_params& pb )
	{
		int32_t data = pb.params[1];
		
		const uint16_t count = pb.params[0];
		
		/*
			     size_code  E  (0, 1,  2)
			1 << size_code  E  (1, 2,  4)
			      
			             n_bytes - 1   E  (0, 1,  3)
			        8 * (n_bytes - 1)  E  (0, 8, 24)
			
			0x80 << 8 * (n_bytes - 1)  E  (0x80, 0x8000, 0x80000000)
		*/
		
		const int n_bytes = 1 << pb.size - 1;
		
		const uint32_t sign_mask = 0x80 << 8 * (n_bytes - 1);
		const uint32_t data_mask = (sign_mask << 1) - 1;
		
		bool last_bit = 0;
		
		uint32_t overflow = 0;
		
		for ( int i = count;  i > 0;  --i )
		{
			overflow |= (data ^ (data << 1)) & sign_mask;
			
			last_bit = data & sign_mask;
			
			s.regs.x = last_bit;
			
			data <<= 1;
		}
		
		s.regs.nzvc = N( data & sign_mask )
		            | Z( (data & data_mask) == 0 )
		            | V( overflow )
		            | C( last_bit );
		
		pb.params[1] = data;
	}
	
	void microcode_LSR( processor_state& s, op_params& pb )
	{
		uint32_t data = pb.params[1];
		
		const uint16_t count = pb.params[0];
		
		/*
			     size_code  E  (0, 1,  2)
			1 << size_code  E  (1, 2,  4)
			      
			             n_bytes - 1   E  (0, 1,  3)
			        8 * (n_bytes - 1)  E  (0, 8, 24)
			
			0x80 << 8 * (n_bytes - 1)  E  (0x80, 0x8000, 0x80000000)
		*/
		
		const int n_bytes = 1 << pb.size - 1;
		
		const uint32_t sign_mask = 0x80 << 8 * (n_bytes - 1);
		const uint32_t data_mask = (sign_mask << 1) - 1;
		
		bool last_bit = 0;
		
		if ( count != 0 )
		{
			data >>= count - 1;
			
			last_bit = data & 0x1;
			
			s.regs.x = last_bit;
			
			data >>= 1;
		}
		
		s.regs.nzvc = N( data & sign_mask )
		            | Z( (data & data_mask) == 0 )
		            | V( 0 )
		            | C( last_bit );
		
		pb.params[1] = data;
	}
	
	void microcode_LSL( processor_state& s, op_params& pb )
	{
		uint32_t data = pb.params[1];
		
		const uint16_t count = pb.params[0];
		
		/*
			     size_code  E  (0, 1,  2)
			1 << size_code  E  (1, 2,  4)
			      
			             n_bytes - 1   E  (0, 1,  3)
			        8 * (n_bytes - 1)  E  (0, 8, 24)
			
			0x80 << 8 * (n_bytes - 1)  E  (0x80, 0x8000, 0x80000000)
		*/
		
		const int n_bytes = 1 << pb.size - 1;
		
		const uint32_t sign_mask = 0x80 << 8 * (n_bytes - 1);
		const uint32_t data_mask = (sign_mask << 1) - 1;
		
		bool last_bit = 0;
		
		if ( count != 0 )
		{
			data <<= count - 1;
			
			last_bit = data & sign_mask;
			
			s.regs.x = last_bit;
			
			data <<= 1;
		}
		
		s.regs.nzvc = N( data & sign_mask )
		            | Z( (data & data_mask) == 0 )
		            | V( 0 )
		            | C( last_bit );
		
		pb.params[1] = data;
	}
	
}

