/*
	microcode.cc
	------------
*/

#include "v68k/microcode.hh"

// v68k
#include "v68k/registers.hh"
#include "v68k/memory.hh"


namespace v68k
{
	
	#define N( x )  (!!(x) << 3)
	#define Z( x )  (!!(x) << 2)
	#define V( x )  (!!(x) << 1)
	#define C( x )  (!!(x) << 0)
	
	
	void microcode_MOVEP_to( registers& regs, const memory& mem, const uint32_t* params )
	{
		const uint32_t mode = params[0];
		const uint32_t x    = params[1];
		const uint32_t y    = params[2];
		const int32_t  disp = params[3];
		
		const uint32_t Dx = regs.d[ x ];
		const uint32_t Ay = regs.a[ y ];
		
		const bool doubled = mode & 0x1;
		
		uint32_t addr = Ay + disp;
		
		switch ( doubled )
		{
			case true:
				mem.put_byte( addr,     Dx >> 24 );
				mem.put_byte( addr + 2, Dx >> 16 );
				
				addr += 4;
				
				// fall through
			
			case false:
				mem.put_byte( addr,     Dx >>  8 );
				mem.put_byte( addr + 2, Dx >>  0 );
		}
	}
	
	void microcode_MOVEP_from( registers& regs, const memory& mem, const uint32_t* params )
	{
		const uint32_t mode = params[0];
		const uint32_t x    = params[1];
		const uint32_t y    = params[2];
		const int32_t  disp = params[3];
		
		uint32_t&      Dx = regs.d[ x ];
		uint32_t const Ay = regs.a[ y ];
		
		const bool doubled = mode & 0x1;
		
		uint32_t addr = Ay + disp;
		
		uint32_t data = doubled ? 0 : Dx & 0xFFFF0000;
		
		switch ( doubled )
		{
			case true:
				data |= mem.get_byte( addr     ) << 24;
				data |= mem.get_byte( addr + 2 ) << 16;
				
				addr += 4;
				
				// fall through
			
			case false:
				data |= mem.get_byte( addr     ) << 8;
				data |= mem.get_byte( addr + 2 ) << 0;
		}
		
		Dx = data;
	}
	
	void microcode_LINK( registers& regs, const memory& mem, const uint32_t* params )
	{
		const uint32_t n    = params[0];
		const int32_t  disp = params[1];
		
		uint32_t& An = regs.a[n];
		uint32_t& sp = regs.a[7];
		
		sp -= 4;
		
		mem.put_long( sp, An );
		
		An = sp;
		
		sp += disp;
	}
	
	void microcode_UNLK( registers& regs, const memory& mem, const uint32_t* params )
	{
		const uint32_t n = params[0];
		
		uint32_t& An = regs.a[n];
		uint32_t& sp = regs.a[7];
		
		sp = An;
		
		An = mem.get_long( sp );
		
		sp += 4;
	}
	
	void microcode_MOVE_to_USP( registers& regs, const memory& mem, const uint32_t* params )
	{
		const uint32_t n = params[0];
		
		regs.usp = regs.a[n];
	}
	
	void microcode_MOVE_from_USP( registers& regs, const memory& mem, const uint32_t* params )
	{
		const uint32_t n = params[0];
		
		regs.a[n] = regs.usp;
	}
	
	void microcode_NOP( registers& regs, const memory& mem, const uint32_t* params )
	{
		// "no operation"
	}
	
	void microcode_MOVEQ( registers& regs, const memory& mem, const uint32_t* params )
	{
		const uint32_t n    = params[0];
		const int32_t  data = params[1];
		
		uint32_t& Dn = regs.d[n];
		
		Dn = data;
		
		regs.nzvc = N( data <  0 )
		          | Z( data == 0 )
		          | V( 0 )
		          | C( 0 );
	}
	
	void microcode_EXG( registers& regs, const memory& mem, const uint32_t* params )
	{
		const uint32_t mode = params[0];
		const uint32_t x    = params[1];  // 3-bit register number
		const uint32_t y    = params[2];  // 4-bit register id
		
		const uint32_t dA = (mode << 3) & mode;  // 0 for D or 8 for A
		
		uint32_t& Rx = regs.d[ dA + x ];
		uint32_t& Ry = regs.d[      y ];
		
		uint32_t temp = Rx;
		
		Rx = Ry;
		
		Ry = temp;
	}
	
}

