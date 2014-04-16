/*
	microcode.cc
	------------
*/

#include "v68k/microcode.hh"

// v68k
#include "v68k/conditional.hh"
#include "v68k/macros.hh"
#include "v68k/state.hh"


#pragma exceptions off


namespace v68k
{
	
	#define N( x )  (!!(x) << 3)
	#define Z( x )  (!!(x) << 2)
	#define V( x )  (!!(x) << 1)
	#define C( x )  (!!(x) << 0)
	
	
	#pragma mark Line 0
	
	void microcode_ORI_to_CCR( processor_state& s, op_params& pb )
	{
		const uint32_t data = pb.first;
		
		s.set_CCR( s.get_CCR() | data );
	}
	
	void microcode_ORI_to_SR( processor_state& s, op_params& pb )
	{
		const uint32_t data = pb.first;
		
		s.set_SR( s.get_SR() | data );
	}
	
	void microcode_BCHG( processor_state& s, op_params& pb )
	{
		pb.result = (1 << pb.first) ^ pb.second;
	}
	
	void microcode_BCLR( processor_state& s, op_params& pb )
	{
		pb.result = ~(1 << pb.first) & pb.second;
	}
	
	void microcode_BSET( processor_state& s, op_params& pb )
	{
		pb.result = (1 << pb.first) | pb.second;
	}
	
	void microcode_MOVEP_to( processor_state& s, op_params& pb )
	{
		const uint32_t x = pb.target;
		
		const uint32_t Dx = s.regs.d[ x ];
		
		uint8_t* p = s.mem.translate( pb.address, (1 << pb.size) - 1, s.data_space(), mem_write );
		
		if ( p == 0 )  // NULL
		{
			s.bus_error();
			
			return;
		}
		
		switch ( (pb.size == long_sized) + 0 )  // clang hates boolean switch conditions
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
		const uint32_t x = pb.target;
		
		uint32_t& Dx = s.regs.d[ x ];
		
		const uint8_t* p = s.mem.translate( pb.address, (1 << pb.size) - 1, s.data_space(), mem_read );
		
		if ( p == 0 )  // NULL
		{
			s.bus_error();
			
			return;
		}
		
		uint32_t data = pb.size == long_sized ? 0 : Dx & 0xFFFF0000;
		
		switch ( (pb.size == long_sized) + 0 )  // clang hates boolean switch conditions
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
		const uint32_t data = pb.first;
		
		s.set_CCR( s.get_CCR() & data );
	}
	
	void microcode_ANDI_to_SR( processor_state& s, op_params& pb )
	{
		const uint32_t data = pb.first;
		
		s.set_SR( s.get_SR() & data );
	}
	
	void microcode_EORI_to_CCR( processor_state& s, op_params& pb )
	{
		const uint32_t data = pb.first;
		
		s.set_CCR( s.get_CCR() ^ data );
	}
	
	void microcode_EORI_to_SR( processor_state& s, op_params& pb )
	{
		const uint32_t data = pb.first;
		
		s.set_SR( s.get_SR() ^ data );
	}
	
	void microcode_MOVES( processor_state& s, op_params& pb )
	{
		const uint32_t more = pb.first;
		
		const int size = byte_count( pb.size );  // 1,2,4
		
		const uint16_t reg_id = more >> 12;
		
		const uint16_t writing = more & 0x0800;
		
		const function_code_t fc = function_code_t( writing ? s.regs.dfc : s.regs.sfc );
		
		const memory_access_t access = writing ? mem_write : mem_read;
		
		uint8_t* p = s.mem.translate( pb.address, size, fc, access );
		
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
				
				default:
					break;
			}
		}
		else
		{
			uint32_t& reg = s.regs.d[ reg_id ];
			
			uint32_t data = 0;
			
			switch ( pb.size )
			{
				case long_sized:
					data |= *p++ << 24;
					data |= *p++ << 16;
				case word_sized:
					data |= *p++ <<  8;
				case byte_sized:
					data |= *p++ <<  0;
				
				default:
					break;
			}
			
			if ( reg_id & 0x8 )
			{
				reg = sign_extend( data, pb.size );
			}
			else
			{
				reg = update( reg, data, pb.size );
			}
		}
	}
	
	#pragma mark -
	#pragma mark Lines 1-3
	
	void microcode_MOVE( processor_state& s, op_params& pb )
	{
		pb.result = pb.first;
	}
	
	#pragma mark -
	#pragma mark Line 4
	
	void microcode_CHK( processor_state& s, op_params& pb )
	{
		const int32_t bound = pb.first;
		const int32_t value = pb.second;
		
		if ( value < 0 )
		{
			s.regs.nzvc = 0x8;  // set N, others undefined
		}
		else if ( value > bound )
		{
			s.regs.nzvc = 0x0;  // clear N, others undefined
		}
		else
		{
			return;  // within bounds
		}
		
		s.take_exception_format_2( 6 * sizeof (uint32_t), s.regs.pc - 2 );
	}
	
	void microcode_LEA( processor_state& s, op_params& pb )
	{
		const uint32_t n = pb.target;
		
		uint32_t& An = s.regs.a[n];
		
		An = pb.address;
	}
	
	void microcode_MOVE_from_CCR( processor_state& s, op_params& pb )
	{
		pb.result = s.get_CCR();
	}
	
	void microcode_MOVE_from_SR( processor_state& s, op_params& pb )
	{
		pb.result = s.get_SR();
	}
	
	void microcode_MOVE_to_CCR( processor_state& s, op_params& pb )
	{
		const uint32_t data = pb.first;
		
		s.set_CCR( data );
	}
	
	void microcode_MOVE_to_SR( processor_state& s, op_params& pb )
	{
		const uint32_t data = pb.first;
		
		s.set_SR( data );
	}
	
	void microcode_SWAP( processor_state& s, op_params& pb )
	{
		const uint32_t n = pb.target;
		
		const uint32_t data = s.regs.d[n];
		
		pb.result   =
		s.regs.d[n] = data << 16
		            | data >> 16;
	}
	
	void microcode_BKPT( processor_state& s, op_params& pb )
	{
		const uint32_t data = pb.target;  // 3-bit breakpoint vector
		
		s.regs.pc -= 2;
		
		s.opcode = 0x4AFC;  // ILLEGAL
		
		if ( bkpt_handler f = s.bkpt )
		{
			s.opcode = f( s, data );
		}
		
		s.condition = processor_condition( bkpt_0 + data );
	}
	
	void microcode_PEA( processor_state& s, op_params& pb )
	{
		uint32_t& sp = s.regs.a[7];
		
		if ( s.badly_aligned_data( sp ) )
		{
			s.address_error();
			
			return;
		}
		
		sp -= 4;
		
		if ( !s.mem.put_long( sp, pb.address, s.data_space() ) )
		{
			s.bus_error();
		}
	}
	
	void microcode_EXT_W( processor_state& s, op_params& pb )
	{
		const uint32_t n = pb.target;
		
		uint32_t& Dn = s.regs.d[n];
		
		const int8_t byte = Dn;
		
		const int16_t word = byte;
		
		Dn = (Dn & 0xFFFF0000) | word;
		
		pb.result = word;
	}
	
	void microcode_EXT_L( processor_state& s, op_params& pb )
	{
		const uint32_t n = pb.target;
		
		uint32_t& Dn = s.regs.d[n];
		
		const int16_t word = Dn;
		
		const int32_t longword = word;
		
		Dn = longword;
		
		pb.result = longword;
	}
	
	void microcode_EXTB( processor_state& s, op_params& pb )
	{
		const uint32_t n = pb.target;
		
		uint32_t& Dn = s.regs.d[n];
		
		const int8_t byte = Dn;
		
		const int32_t longword = byte;
		
		Dn = longword;
		
		pb.result = longword;
	}
	
	void microcode_TAS( processor_state& s, op_params& pb )
	{
		const int32_t data = sign_extend( pb.second, byte_sized );
		
		s.regs.nzvc = N( data <  0 )
		            | Z( data == 0 )
		            | V( 0 )
		            | C( 0 );
		
		pb.result = data | 0x80;
	}
	
	void microcode_MOVEM_to( processor_state& s, op_params& pb )
	{
		uint16_t mask = pb.first;
		uint32_t addr = pb.address;
		
		const uint32_t update_register = pb.target;
		
		const bool longword_sized = pb.size == long_sized;
		
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
		uint16_t mask = pb.first;
		uint32_t addr = pb.address;
		
		const uint32_t update_register = pb.target;
		
		const bool longword_sized = pb.size == long_sized;
		
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
		const uint32_t trap_number = pb.first;
		
		s.take_exception_format_0( (trap_number + 32) * sizeof (uint32_t) );
	}
	
	void microcode_LINK( processor_state& s, op_params& pb )
	{
		const uint32_t n    = pb.target;
		const int32_t  disp = pb.first;
		
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
		const uint32_t n = pb.target;
		
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
		const uint32_t n = pb.target;
		
		// MOVE USP is privileged, so USP is never A7 here
		s.regs.alt_sp = s.regs.a[n];
	}
	
	void microcode_MOVE_from_USP( processor_state& s, op_params& pb )
	{
		const uint32_t n = pb.target;
		
		// MOVE USP is privileged, so USP is never A7 here
		s.regs.a[n] = s.regs.alt_sp;
	}
	
	void microcode_NOP( processor_state& s, op_params& pb )
	{
		// "no operation"
	}
	
	void microcode_STOP( processor_state& s, op_params& pb )
	{
		const uint32_t data = pb.first;
		
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
		
		if ( !s.mem.get_word( sp + 6, id, supervisor_data_space ) )
		{
			s.bus_error();
			
			return;
		}
		
		const uint16_t format = id >> 12;
		
		const uint32_t stack_frame_size = format == 0 ?  8
		                                : format == 2 ? 12
		                                :                0;
		
		if ( stack_frame_size == 0 )
		{
			s.format_error();
			
			return;
		}
		
		if ( !s.mem.get_long( sp + 2, s.regs.pc, supervisor_data_space ) )
		{
			s.bus_error();
			
			return;
		}
		
		uint16_t saved_sr;
		
		if ( !s.mem.get_word( sp, saved_sr, supervisor_data_space ) )
		{
			s.bus_error();
			
			return;
		}
		
		sp += stack_frame_size;
		
		s.set_SR( saved_sr );
	}
	
	void microcode_RTD( processor_state& s, op_params& pb )
	{
		microcode_RTS( s, pb );
		
		s.regs.a[7] += pb.first;
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
			s.take_exception_format_2( 7 * sizeof (uint32_t), s.regs.pc - 2 );
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
		return id >> 8 | (id & ~0x0800);
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
		const uint32_t registers = pb.first;
		const uint32_t writing   = pb.second;
		
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
	
	#pragma mark -
	#pragma mark Line 5
	
	void microcode_DBcc( processor_state& s, op_params& pb )
	{
		const uint16_t cc = pb.second;
		
		if ( test_conditional( cc, s.regs.nzvc ) )
		{
			return;
		}
		
		const uint32_t n = pb.target;
		
		const int16_t counter = s.regs.d[ n ];
		
		if ( counter == 0 )
		{
			// decrement low word only, which is zero
			
			s.regs.d[ n ] += 0x0000FFFF;
		}
		else
		{
			s.regs.d[ n ] -= 1;  // decrement of non-zero word won't borrow
			
			s.regs.pc = pb.address;
		}
	}
	
	void microcode_Scc( processor_state& s, op_params& pb )
	{
		const uint16_t cc = pb.second;
		
		pb.result = int32_t() - test_conditional( cc, s.regs.nzvc );
	}
	
	#pragma mark -
	#pragma mark Line 6
	
	void microcode_BRA( processor_state& s, op_params& pb )
	{
		s.regs.pc = pb.address;
	}
	
	void microcode_BSR( processor_state& s, op_params& pb )
	{
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
		
		s.regs.pc = pb.address;
	}
	
	void microcode_Bcc( processor_state& s, op_params& pb )
	{
		const uint16_t cc = pb.second;
		
		if ( test_conditional( cc, s.regs.nzvc ) )
		{
			s.regs.pc = pb.address;
		}
	}
	
	#pragma mark -
	#pragma mark Line 8
	
	void microcode_OR( processor_state& s, op_params& pb )
	{
		pb.result = pb.first | pb.second;
	}
	
	static uint32_t BCD_encoded( int x )
	{
		return + x / 10 << 4
		       | x % 10;
	}
	
	static int decoded_BCD( uint8_t x )
	{
		return 10 * (x >> 4) + (x & 0xF);
	}
	
	void microcode_SBCD( processor_state& s, op_params& pb )
	{
		const int a = decoded_BCD( pb.first  );
		const int b = decoded_BCD( pb.second );
		
		pb.result = BCD_encoded( (100 + b - a) % 100 );
	}
	
	void microcode_DIVS( processor_state& s, op_params& pb )
	{
		const int32_t dividend = pb.second;
		const int16_t divisor  = pb.first;
		
		if ( divisor == 0 )
		{
			s.take_exception_format_2( 5 * sizeof (uint32_t), pb.address );
			
			pb.size = unsized;  // Don't store the result
			
			return;
		}
		
		const int32_t quotient_32 = dividend / divisor;
		
		const int16_t quotient = quotient_32;
		
		if ( quotient != quotient_32 )
		{
			pb.size = unsized;  // Don't store the result
		}
		else
		{
			//const int16_t remainder = dividend % divisor;
			const int16_t remainder = dividend - quotient * divisor;
			
			pb.result = remainder << 16 | uint16_t( quotient );
		}
	}
	
	void microcode_DIVU( processor_state& s, op_params& pb )
	{
		const uint32_t dividend = pb.second;
		const uint16_t divisor  = pb.first;
		
		if ( divisor == 0 )
		{
			s.take_exception_format_2( 5 * sizeof (uint32_t), pb.address );
			
			pb.size = unsized;  // Don't store the result
			
			return;
		}
		
		const uint32_t quotient_32 = dividend / divisor;
		
		const uint16_t quotient = quotient_32;
		
		if ( quotient != quotient_32 )
		{
			pb.size = unsized;  // Don't store the result
		}
		else
		{
			//const uint16_t remainder = dividend % divisor;
			const uint16_t remainder = dividend - quotient * divisor;
			
			pb.result = remainder << 16 | quotient;
		}
	}
	
	#pragma mark -
	#pragma mark Line 9
	
	void microcode_SUB( processor_state& s, op_params& pb )
	{
		const int32_t a = pb.first;
		const int32_t b = pb.second;
		
		const int32_t diff = b - a;
		
		pb.result = diff;
	}
	
	#pragma mark -
	#pragma mark Line B
	
	void microcode_EOR( processor_state& s, op_params& pb )
	{
		pb.result = pb.first ^ pb.second;
	}
	
	#pragma mark -
	#pragma mark Line C
	
	void microcode_AND( processor_state& s, op_params& pb )
	{
		pb.result = pb.first & pb.second;
	}
	
	void microcode_EXG( processor_state& s, op_params& pb )
	{
		// Actually, these are swapped.  For "EXG A4,D0", x == 0 and y == 12.
		const uint32_t x = pb.first;
		const uint32_t y = pb.second;
		
		uint32_t& Rx = s.regs.d[ x ];
		uint32_t& Ry = s.regs.d[ y ];
		
		uint32_t temp = Rx;
		
		Rx = Ry;
		
		Ry = temp;
	}
	
	void microcode_ABCD( processor_state& s, op_params& pb )
	{
		const int a = decoded_BCD( pb.first  );
		const int b = decoded_BCD( pb.second );
		
		const int sum = a + b;
		
		pb.result = BCD_encoded( sum % 100 );
		
		if ( const bool carry = sum >= 100 )
		{
			// Sick hack so ADDX-style CCR update sets the C bit
			pb.first  |= 0x80;
			pb.second |= 0x80;
		}
	}
	
	void microcode_MULS( processor_state& s, op_params& pb )
	{
		pb.result = int16_t( pb.first ) * int16_t( pb.second );
		
		pb.size = long_sized;
	}
	
	void microcode_MULU( processor_state& s, op_params& pb )
	{
		pb.result = uint16_t( pb.first ) * uint16_t( pb.second );
		
		pb.size = long_sized;
	}
	
	#pragma mark -
	#pragma mark Line D
	
	void microcode_ADD( processor_state& s, op_params& pb )
	{
		const int32_t a = pb.first;
		const int32_t b = pb.second;
		
		const int32_t sum = a + b;
		
		pb.result = sum;
	}
	
	void microcode_ASR( processor_state& s, op_params& pb )
	{
		int32_t data = pb.second;
		
		const uint16_t count = pb.first;
		
		bool last_bit = 0;
		
		if ( count != 0 )
		{
			if ( count > 32 )
			{
				data = 0;
			}
			else
			{
				data >>= count - 1;
				
				last_bit = data & 0x1;
				
				data >>= 1;
			}
			
			s.regs.x = last_bit;
		}
		
		s.regs.nzvc = N( data <  0 )
		            | Z( data == 0 )
		            | V( 0 )
		            | C( last_bit );
		
		pb.result = data;
	}
	
	void microcode_ASL( processor_state& s, op_params& pb )
	{
		const uint16_t count = pb.first;
		
		int32_t data = pb.second;
		
		bool overflow = 0;
		bool last_bit = 0;
		
		if ( count != 0 )
		{
			if ( count >= 32 )
			{
				overflow = data ^ (data << 1);
				
				if ( count == 32 )
				{
					last_bit = data & 0x1;
				}
				
				data = 0;
			}
			else
			{
				// e.g. 0x0000FFFF
				const uint32_t data_mask = zero_extend( 0xFFFFFFFF, pb.size );
				
				// bshift:  0x00003FFF
				// negate:  0xFFFFC000
				// extend:  0x0000C000
				const uint32_t overflow_mask = zero_extend( ~( data_mask >> count ), pb.size );
				
				overflow = (data ^ (data << 1)) & overflow_mask;
				
				data <<= count - 1;
				
				last_bit = sign_extend( data, pb.size ) < 0;
				
				data <<= 1;
				
				data = sign_extend( data, pb.size );
			}
			
			s.regs.x = last_bit;
		}
		
		s.regs.nzvc = N( data <  0 )
		            | Z( data == 0 )
		            | V( overflow )
		            | C( last_bit );
		
		pb.result = data;
	}
	
	void microcode_LSR( processor_state& s, op_params& pb )
	{
		const uint16_t count = pb.first;
		
		int32_t data = pb.second;
		
		bool last_bit = 0;
		
		if ( count != 0 )
		{
			if ( count <= 32 )
			{
				uint32_t udata = zero_extend( data, pb.size );
				
				udata >>= count - 1;
				
				last_bit = data & 0x1;
				
				udata >>= 1;
				
				data = udata;
			}
			else
			{
				data = 0;
			}
			
			s.regs.x = last_bit;
		}
		
		s.regs.nzvc = N( data <  0 )
		            | Z( data == 0 )
		            | V( 0 )
		            | C( last_bit );
		
		pb.result = data;
	}
	
	void microcode_LSL( processor_state& s, op_params& pb )
	{
		const uint16_t count = pb.first;
		
		int32_t data = pb.second;
		
		bool last_bit = 0;
		
		if ( count != 0 )
		{
			if ( count <= 32 )
			{
				data = zero_extend( data, pb.size );
				
				data <<= count - 1;
				
				last_bit = sign_extend( data, pb.size ) < 0;
				
				data <<= 1;
				
				data = sign_extend( data, pb.size );
			}
			else
			{
				data = 0;
			}
			
			s.regs.x = last_bit;
		}
		
		s.regs.nzvc = N( data <  0 )
		            | Z( data == 0 )
		            | V( 0 )
		            | C( last_bit );
		
		pb.result = data;
	}
	
	void microcode_ROXR( processor_state& s, op_params& pb )
	{
		const uint32_t count = pb.first;
		
		int32_t data = pb.second;
		
		if ( count != 0 )
		{
			const int n_bits = bit_count( pb.size );
			
			const int effective_count = count % (n_bits + 1);
			
			if ( effective_count != 0 )
			{
				const int anticount = n_bits - effective_count;
				
				const uint32_t udata = zero_extend( data, pb.size );
				
				data = (udata << 1 | s.regs.x) << anticount
				     |  udata                  >> effective_count;
				
				s.regs.x = udata >> (effective_count - 1 ) & 0x1;
				
				data = sign_extend( data, pb.size );
			}
		}
		
		s.regs.nzvc = N( data <  0 )
		            | Z( data == 0 )
		            | V( 0 )
		            | C( s.regs.x );
		
		pb.result = data;
	}
	
	void microcode_ROXL( processor_state& s, op_params& pb )
	{
		const uint32_t count = pb.first;
		
		int32_t data = pb.second;
		
		if ( count != 0 )
		{
			const int n_bits = bit_count( pb.size );
			
			const int effective_count = count % (n_bits + 1);
			
			if ( effective_count != 0 )
			{
				const int anticount = n_bits - effective_count;
				
				const uint32_t udata = zero_extend( data, pb.size );
				
				data = (udata << 1 | s.regs.x) << (effective_count - 1)
				     |  udata                  >> (anticount + 1);
				
				s.regs.x = udata >> anticount & 0x1;
				
				data = sign_extend( data, pb.size );
			}
		}
		
		s.regs.nzvc = N( data <  0 )
		            | Z( data == 0 )
		            | V( 0 )
		            | C( s.regs.x );
		
		pb.result = data;
	}
	
	void microcode_ROR( processor_state& s, op_params& pb )
	{
		const uint32_t count = pb.first;
		
		int32_t data = pb.second;
		
		bool last_bit = 0;
		
		if ( count != 0 )
		{
			const int n_bits = bit_count( pb.size );
			
			const int effective_count = count & n_bits - 1;
			
			if ( effective_count != 0 )
			{
				const int anticount = n_bits - effective_count;
				
				const uint32_t udata = zero_extend( data, pb.size );
				
				data = udata << anticount
				     | udata >> effective_count;
				
				data = sign_extend( data, pb.size );
			}
			
			last_bit = data < 0;
		}
		
		s.regs.nzvc = N( data <  0 )
		            | Z( data == 0 )
		            | V( 0 )
		            | C( last_bit );
		
		pb.result = data;
	}
	
	void microcode_ROL( processor_state& s, op_params& pb )
	{
		const uint32_t count = pb.first;
		
		int32_t data = pb.second;
		
		bool last_bit = 0;
		
		if ( count != 0 )
		{
			const int n_bits = bit_count( pb.size );
			
			const int effective_count = count & n_bits - 1;
			
			if ( effective_count != 0 )
			{
				const int anticount = n_bits - effective_count;
				
				const uint32_t udata = zero_extend( data, pb.size );
				
				data = udata << effective_count
				     | udata >> anticount;
				
				data = sign_extend( data, pb.size );
			}
			
			last_bit = data & 0x1;
		}
		
		s.regs.nzvc = N( data <  0 )
		            | Z( data == 0 )
		            | V( 0 )
		            | C( last_bit );
		
		pb.result = data;
	}
	
}

