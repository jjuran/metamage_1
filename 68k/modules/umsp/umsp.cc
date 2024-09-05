/*
	umsp.cc
	-------
	
	User-Mode Service Package for xv68k
	
	On a 68010, MOVE from SR is privileged.  The 68010 adds MOVE from CCR for
	non-privileged code to use, but this is too late for code that must also
	be compatible with the 68000 (which lacks MOVE from CCR).
	
	Solution:  Continue to use MOVE from SR in code running in user-mode, and
	emulate it in the Privilege Violation exception handler.
*/

// Standard C
#include <stdint.h>

// iota
#include "iota/endian.hh"


#pragma exceptions off


uint8_t CPUFlag : 0x012F;

struct registers
{
	uint32_t d[8];  // D0-D7
	uint32_t a[8];  // A0-A7
	
	uint16_t sr;
	uint32_t pc;
	uint16_t fv;
};

static uint32_t vector_array[ 256 ];

inline uint16_t& select_word( uint32_t& longword, int i )
{
	return ((uint16_t*) &longword)[ i == iota::is_little_endian() ];
}

inline uint16_t& low_word ( uint32_t& x )  { return select_word( x, 0 ); }

static inline
uint16_t read_word( uint32_t& address )
{
	const uint16_t*& addr = (const uint16_t*&) address;
	
	return *addr++;
}

static inline
void rewind_word( uint32_t& address )
{
	address -= 2;
}

static
uint16_t& resolve_ea16( registers& regs, const uint16_t ea )
{
	if ( ea < (2 << 3) )
	{
		// Dn or An
		return low_word( regs.d[ ea ] );
	}
	
	uint32_t addr;
	
	if ( ea == (7 << 3 | 0) )
	{
		addr = (int32_t)(int16_t) read_word( regs.pc );
	}
	else if ( ea == (7 << 3 | 1) )
	{
		addr  = read_word( regs.pc ) << 16;
		addr |= read_word( regs.pc );
	}
	else if ( ea == (7 << 3 | 4) )
	{
		addr = regs.pc;
		
		regs.pc += 2;
	}
	else
	{
		const uint16_t mode = ea >> 3;
		const uint16_t N    = ea & 0x7;
		
		if ( mode == 4 )
		{
			regs.a[ N ] -= 2;
		}
		
		addr = regs.a[ N ];
		
		if ( mode == 3 )
		{
			regs.a[ N ] += 2;
		}
		
		if ( mode == 5 )
		{
			addr += (int16_t) read_word( regs.pc );
		}
		
		if ( mode == 6 )
		{
			const uint16_t extension = read_word( regs.pc );
			
			const uint16_t _n = extension >> 12;
			
			int32_t index = regs.d[ _n ];  // Xn
			
			const bool long_sized = extension >> 11 & 0x1;
			
			if ( ! long_sized )
			{
				index = int16_t( index );
			}
			
			addr += (int8_t) extension;
			addr += index;
			
			// TODO:  68020 scaling and full format
		}
	}
	
	return *(uint16_t*) addr;
}

static
bool handle_exception( registers& regs )
{
	const uint32_t old_pc = regs.pc;
	
	const uint16_t opcode = read_word( regs.pc );
	
	// 0100 0000 11xx xxxx:  MOVE from SR
	
	if ( (opcode & 0xFFC0) == 0x40C0 )
	{
		resolve_ea16( regs, opcode & 0x003F ) = regs.sr;
		
		return true;
	}
	
	// 0100 0110 11xx xxxx:  MOVE to SR
	
	if ( (opcode & 0xFFC0) == 0x46C0 )
	{
		uint16_t bits = resolve_ea16( regs, opcode & 0x003F );
		
		if ( ((regs.sr ^ bits) & 0xF000) == 0 )
		{
			// We allow user-mode to alter the interrupt mask only
			
			regs.sr = bits;
		}
		// else do nothing
		
		return true;
	}
	
	// 0000 0000 0111 1100:  ORI to SR
	
	if ( opcode == 0x007C )
	{
		uint16_t bits = read_word( regs.pc );
		
		if ( (bits & 0xF000) == 0 )
		{
			// We allow user-mode to alter the interrupt mask only
			
			regs.sr |= bits;
			
			return true;
		}
	}
	
	// 0000 0010 0111 1100:  ANDI to SR
	
	if ( opcode == 0x027C )
	{
		uint16_t bits = read_word( regs.pc );
		
		if ( (bits & 0xFF00) == 0xFF00 )
		{
			// We allow user-mode to alter the CCR only
			
			regs.sr &= bits;
			
			return true;
		}
	}
	
	regs.pc = old_pc;
	
	return false;
}

/*
	On a 68000, we install our exception handler at address $0020
	(i.e. with a vector base of zero) after saving the handler
	previously installed there.  If we don't adequately emulate
	the privileged instruction, we jump to the previous handler.
*/

static void* previous_exception_handler;

static
asm void MC68000_Privilege_Violation_exception_handler()
{
	MOVEM.L  D0-D7/A0-A7,-(SP)
	MOVE     USP,A0
	MOVE.L   A0,60(SP)          // copy user SP to registers
	
	MOVE.L   SP,-(SP)
	
	JSR      handle_exception
	TST.B    D0
	
	ADDQ.L   #4,SP
	
	MOVEA.L  60(SP),A0          // update USP
	MOVE     A0,USP
	MOVEM.L  (SP)+,D0-D7/A0-A6  // update registers (not A7)
	ADDQ.L   #4,SP
	
	BEQ.S    unhandled
	
	RTE
	
unhandled:
	
	// jump to previous exception handler without clobbering registers
	MOVE.L   previous_exception_handler,-(SP)
	RTS
}

/*
	On a 68010 or later, we do things a bit differently.
	
	The MOVE from SR instruction is privileged, allowing for
	proper virtualization (such that kernel code running in
	a virtual machine might be led to believe that it's in
	Supervisor mode when it's actually not).
	
	Our privileged instruction emulation covers this, but
	we run into a snag with MacRelix, which installs its own
	Privilege Violation exception handler /and/ executes
	MOVE from SR (as part of system-info).  Since MacRelix's
	exception handler displaces any previously installed one,
	the system-info process dies on SIGILL, which is not a
	nominal outcome.
	
	The solution is to apply the Vector Base Register, pointing
	it to a different block of memory that MacRelix won't touch.
	We mostly populate it with a generic exception handler that
	merely jumps through the corresponding low memory vector.
	The Privilege Violation exception handler is mostly the same
	as for the 68000, except that the exception-unhandled path
	falls back to the generic exception handler instead of one
	specific handler address saved from low memory at setup time
	as in the 68000 case.  (It would be wrong to do so, since it
	would prevent MacRelix's Privilege Violation exception handler
	from ever getting called, which would be the opposite problem.)
*/

static
asm void default_exception_handler()
{
	MOVEM.L  D0/A0-A1,-(SP)
	
	MOVE.W   18(SP),D0    // skip 3 saved regs, SR, PC -> format/vector word
	
	AND.W    #0x3FF,D0    // just the vector offset
	MOVEA.W  D0,A0        // make it a vector address
	MOVEA.L  (A0),A0      // read the exception handler address
	
	MOVE.L   A0,8(SP)     // overwrite saved A1 value with next address
	
	MOVEM.L  (SP)+,D0/A0  // restore registers (not A1)
	
	RTS                   // jump to lowmem exception handler
}

static
asm void VBR_Privilege_Violation_exception_handler()
{
	MOVEM.L  D0-D7/A0-A7,-(SP)
	MOVE     USP,A0
	MOVE.L   A0,60(SP)          // copy user SP to registers
	
	MOVE.L   SP,-(SP)
	
	JSR      handle_exception
	TST.B    D0
	
	ADDQ.L   #4,SP
	
	MOVEA.L  60(SP),A0          // update USP
	MOVE     A0,USP
	MOVEM.L  (SP)+,D0-D7/A0-A6  // update registers (not A7)
	ADDQ.L   #4,SP
	
	BEQ.S    unhandled
	
	RTE
	
unhandled:
	
	JMP      default_exception_handler
}

static
asm int set_exception_handler()
{
	machine 68010
	
	MOVE.B  CPUFlag,D7  // read lowmem while still in User mode
	
	JSR     0xFFFFFFFA ;  // enter_supervisor_mode()
	
	BMI.S   bail ;  // D0 is -1 if branch taken
	
	TST.B   D7
	BEQ.S   no_VBR
	
	LEA     default_exception_handler,A0
	LEA     vector_array,A1
	MOVEA.L A1,A2
	
	MOVE.W  #255,D6
loop:
	MOVE.L  A0,(A1)+
	DBRA    D6,loop
	
	MOVEC   A2,VBR
	
	LEA     VBR_Privilege_Violation_exception_handler,A0
	MOVE.L  A0,0x0020(A2)
	BRA.S   rejoin
	
no_VBR:
	MOVE.L  0x0020,previous_exception_handler
	
	LEA     MC68000_Privilege_Violation_exception_handler,A0
	
	MOVE.L  A0,0x0020  // Privilege Violation
	
rejoin:
	MOVE    D0,SR
	MOVEQ   #0,D0
	
bail:
	RTS
}

int asm main( int argc, char** argv )
{
	LINK     A6,#0
	
	JSR      set_exception_handler
	BMI.S    bail
	
	JSR      0xFFFFFFF8  // module-suspend
	
	// not reached
	
bail:
	MOVEQ.L  #1,D0
	UNLK     A6
	RTS
}
