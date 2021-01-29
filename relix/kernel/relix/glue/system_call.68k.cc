/*
	system_call.68k.cc
	------------------
*/

#include "relix/glue/system_call.68k.hh"

// Relix
#include "relix/api/current_stack_base.hh"
#include "relix/config/syscall_stacks.hh"
#include "relix/glue/kernel_boundary.hh"
#include "relix/syscall/registry.hh"


namespace relix
{
	
#ifdef __MC68K__
	
	asm void dispatch_68k_system_call( ... )
	{
		// D0 contains the system call number
		
		LINK     A6,#0
		MOVEM.L  D0/D3-D7/A2-A4,-(SP)  // save context regs, push syscall number
		
	#if CONFIG_SYSCALL_STACKS
		
		JSR      current_stack_base
		
		MOVE.L   A0,D0    // test the result
		BEQ.S    restart  // skip stack setup if we don't have one
		
		// copy things from current stack to new stack
		
		SUBQ     #8,A0  // point after return address, to stack limit
		
		MOVE.L   4(A6),-(A0)  // return address
		MOVE.L    (A6),-(A0)  // saved frame pointer (backlink)
		
		MOVE.L    (SP),-(A0)  // system call number
		
		MOVEA.L  A0,SP  // switch to new stack
		
	#endif
		
	restart:
		
		JSR      enter_system_call  // returns syscall number in D0
		
		CMP.W    the_last_syscall,D0
		BLT      in_range
		
		MOVE.W   the_last_syscall,D0
		
	in_range:
		MOVEA.L  the_syscall_array,A0
		
	#if __MC68020__
		MOVEA.L  (A0,D0.W*8),A0
	#else
		LSL.W    #3,D0
		MOVEA.L  (A0,D0.W),A0
	#endif
		
		// copy args onto the stack
		MOVEM.L  8(A6),D0-D5
		MOVEM.L  D0-D5,-(SP)
		
		JSR      (A0)
		
		ADDA.L   #24,SP
		
		MOVE.L   D0,-(SP)  // save D0
		
		JSR      leave_system_call
		
		MOVE.L   D0,D1  // restart the system call?
		
		MOVE.L   (SP)+,D0  // restore D0
		
		TST.L    D1  // restart the system call?
		
		BNE.S    restart
		
		MOVEM.L  -32(A6),D3-D7/A2-A4  // restore context
		UNLK     A6
		
		RTS
	}
	
#elif defined( __APPLE__ )
	
	void dummy()
	{
	}
	
#endif
	
}
