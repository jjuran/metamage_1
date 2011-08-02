/*
	microcode.cc
	------------
*/

#include "v68k/microcode.hh"

// v68k
#include "v68k/state.hh"


namespace v68k
{
	
	#define N( x )  (!!(x) << 3)
	#define Z( x )  (!!(x) << 2)
	#define V( x )  (!!(x) << 1)
	#define C( x )  (!!(x) << 0)
	
	
	void microcode_MOVEP_to( processor_state& s, const uint32_t* params )
	{
		const uint32_t mode = params[0];
		const uint32_t x    = params[1];
		const uint32_t y    = params[2];
		const int32_t  disp = params[3];
		
		const uint32_t Dx = s.regs.d[ x ];
		const uint32_t Ay = s.regs.a[ y ];
		
		const bool doubled = mode & 0x1;
		
		uint32_t addr = Ay + disp;
		
		switch ( doubled )
		{
			case true:
				s.mem.put_byte( addr,     Dx >> 24 );
				s.mem.put_byte( addr + 2, Dx >> 16 );
				
				addr += 4;
				
				// fall through
			
			case false:
				s.mem.put_byte( addr,     Dx >>  8 );
				s.mem.put_byte( addr + 2, Dx >>  0 );
		}
	}
	
	void microcode_MOVEP_from( processor_state& s, const uint32_t* params )
	{
		const uint32_t mode = params[0];
		const uint32_t x    = params[1];
		const uint32_t y    = params[2];
		const int32_t  disp = params[3];
		
		uint32_t&      Dx = s.regs.d[ x ];
		uint32_t const Ay = s.regs.a[ y ];
		
		const bool doubled = mode & 0x1;
		
		uint32_t addr = Ay + disp;
		
		uint32_t data = doubled ? 0 : Dx & 0xFFFF0000;
		
		switch ( doubled )
		{
			case true:
				data |= s.mem.get_byte( addr     ) << 24;
				data |= s.mem.get_byte( addr + 2 ) << 16;
				
				addr += 4;
				
				// fall through
			
			case false:
				data |= s.mem.get_byte( addr     ) << 8;
				data |= s.mem.get_byte( addr + 2 ) << 0;
		}
		
		Dx = data;
	}
	
	void microcode_LEA( processor_state& s, const uint32_t* params )
	{
		const uint32_t addr = params[0];
		const uint32_t n    = params[1];
		
		uint32_t& An = s.regs.a[n];
		
		An = addr;
	}
	
	void microcode_BKPT( processor_state& s, const uint32_t* params )
	{
		const uint32_t data = params[0];  // 3-bit breakpoint vector
		
		s.regs.pc -= 2;
		
		s.condition = processor_condition( bkpt_0 + data );
	}
	
	void microcode_PEA( processor_state& s, const uint32_t* params )
	{
		const uint32_t addr = params[0];
		
		uint32_t& sp = s.regs.a[7];
		
		sp -= 4;
		
		s.mem.put_long( sp, addr );
	}
	
	void microcode_EXT_W( processor_state& s, const uint32_t* params )
	{
		const uint32_t n = params[0];
		
		uint32_t& Dn = s.regs.d[n];
		
		const int8_t byte = Dn;
		
		const int16_t word = byte;
		
		Dn = (Dn & 0xFFFF0000) | word;
		
		s.regs.nzvc = N( word <  0 )
		            | Z( word == 0 )
		            | V( 0 )
		            | C( 0 );
	}
	
	void microcode_EXT_L( processor_state& s, const uint32_t* params )
	{
		const uint32_t n = params[0];
		
		uint32_t& Dn = s.regs.d[n];
		
		const int16_t word = Dn;
		
		const int32_t longword = word;
		
		Dn = longword;
		
		s.regs.nzvc = N( longword <  0 )
		            | Z( longword == 0 )
		            | V( 0 )
		            | C( 0 );
	}
	
	void microcode_EXTB( processor_state& s, const uint32_t* params )
	{
		const uint32_t n = params[0];
		
		uint32_t& Dn = s.regs.d[n];
		
		const int8_t byte = Dn;
		
		const int32_t longword = byte;
		
		Dn = longword;
		
		s.regs.nzvc = N( longword <  0 )
		            | Z( longword == 0 )
		            | V( 0 )
		            | C( 0 );
	}
	
	void microcode_LINK( processor_state& s, const uint32_t* params )
	{
		const uint32_t n    = params[0];
		const int32_t  disp = params[1];
		
		uint32_t& An = s.regs.a[n];
		uint32_t& sp = s.regs.a[7];
		
		sp -= 4;
		
		s.mem.put_long( sp, An );
		
		An = sp;
		
		sp += disp;
	}
	
	void microcode_UNLK( processor_state& s, const uint32_t* params )
	{
		const uint32_t n = params[0];
		
		uint32_t& An = s.regs.a[n];
		uint32_t& sp = s.regs.a[7];
		
		sp = An;
		
		An = s.mem.get_long( sp );
		
		sp += 4;
	}
	
	void microcode_MOVE_to_USP( processor_state& s, const uint32_t* params )
	{
		const uint32_t n = params[0];
		
		// MOVE USP is privileged, so USP is never A7 here
		s.regs.alt_sp = s.regs.a[n];
	}
	
	void microcode_MOVE_from_USP( processor_state& s, const uint32_t* params )
	{
		const uint32_t n = params[0];
		
		// MOVE USP is privileged, so USP is never A7 here
		s.regs.a[n] = s.regs.alt_sp;
	}
	
	void microcode_NOP( processor_state& s, const uint32_t* params )
	{
		// "no operation"
	}
	
	void microcode_STOP( processor_state& s, const uint32_t* params )
	{
		const uint32_t data = params[0];
		
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
	
	void microcode_RTS( processor_state& s, const uint32_t* params )
	{
		uint32_t& sp = s.regs.a[7];
		
		s.regs.pc = s.mem.get_long( sp );
		
		sp += 4;
	}
	
	void microcode_BRA( processor_state& s, const uint32_t* params )
	{
		const uint32_t pc   = params[0];
		const int32_t  disp = params[1];
		
		s.regs.pc = pc + disp;
	}
	
	void microcode_BSR( processor_state& s, const uint32_t* params )
	{
		const uint32_t pc   = params[0];
		const int32_t  disp = params[1];
		
		uint32_t& sp = s.regs.a[7];
		
		sp -= 4;
		
		s.mem.put_long( sp, s.regs.pc );
		
		s.regs.pc = pc + disp;
	}
	
	void microcode_MOVEQ( processor_state& s, const uint32_t* params )
	{
		const uint32_t n    = params[0];
		const int32_t  data = params[1];
		
		uint32_t& Dn = s.regs.d[n];
		
		Dn = data;
		
		s.regs.nzvc = N( data <  0 )
		            | Z( data == 0 )
		            | V( 0 )
		            | C( 0 );
	}
	
	void microcode_EXG( processor_state& s, const uint32_t* params )
	{
		const uint32_t mode = params[0];
		const uint32_t x    = params[1];  // 3-bit register number
		const uint32_t y    = params[2];  // 4-bit register id
		
		const uint32_t dA = (mode << 3) & mode;  // 0 for D or 8 for A
		
		uint32_t& Rx = s.regs.d[ dA + x ];
		uint32_t& Ry = s.regs.d[      y ];
		
		uint32_t temp = Rx;
		
		Rx = Ry;
		
		Ry = temp;
	}
	
}

