/*
	system_call.ppc.cc
	------------------
*/

#include "relix/glue/system_call.ppc.hh"

// relix
#include "relix/restack.h"
#include "relix/config/syscall_stacks.hh"
#include "relix/glue/kernel_boundary.hh"
#include "relix/syscall/registry.hh"


extern "C" void* current_stack_base();


namespace relix
{
	
#ifdef __POWERPC__
#ifndef __MWERKS__
	
	void dispatch_ppc_system_call( ... )
	{
		// FIXME?
	}
	
#else
	
	enum { kSystemCallSize = sizeof (system_call) };
	
	asm void dispatch_ppc_system_call( ... )
	{
		nofralloc
		
		// standard prolog
		mflr    r0
		stw     r0,8(SP)
		
		// allocate a stack frame
		stwu    SP,-144(SP)
		
		// save up to 6 parameters
		stw     r5,32(SP)
		stw     r6,36(SP)
		stw     r7,40(SP)
		stw     r8,44(SP)
		stw     r3,48(SP)
		stw     r4,52(SP)
		
		// save the system call number in the slot reserved for binder use
		stw     r11,16(SP)
		
	#if CONFIG_SYSCALL_STACKS
		
		stmw    r13,64(SP)
		
		bl      current_stack_base
		
		cmpi    cr0,r3,0
		
		beq+    cr0,restart
		
		addi    r12,r3,(-sizeof (_relix_stack_footer))  // r12 <= &stack.frame_pointer
		
		// reload syscall number, load backlink
		lwz     r11,16(SP)
		lwz     r10, 0(SP)
		
		// reload parameters
		lwz     r5,32(SP)
		lwz     r6,36(SP)
		lwz     r7,40(SP)
		lwz     r8,44(SP)
		lwz     r3,48(SP)
		lwz     r4,52(SP)
		
		// save syscall number and backlink in syscall stack
		stw     r11,16(r12)
		stw     r10, 0(r12)
		
		// save parameters in syscall stack
		stw     r5,32(r12)
		stw     r6,36(r12)
		stw     r7,40(r12)
		stw     r8,44(r12)
		stw     r3,48(r12)
		stw     r4,52(r12)
		
		mr      SP,r12
		
	#endif
		
	restart:
		
		// load syscall number as an argument to enter_system_call()
	//	mr      r3,r11
		lwz     r3,16(SP)
		
		bl      enter_system_call  // returns syscall number
		
		// r3 contains the requested system call number
		
		// r12.last = gLastSystemCall;
		lwz     r12,the_last_syscall
		lwz     r12,0(r12)
		
		// if ( r3.index > r12.last )
		cmpl    cr0,r3,r12
		blt+    cr0,in_range
		// r3.index = r12.last;
		mr      r3,r12
		
	in_range:
		
		// r12.array = gSystemCallArray;
		lwz     r12,the_syscall_array
		lwz     r12,0(r12)
		
		// r12.f = r12.array[ r3.index ].function
		mulli   r3,r3,kSystemCallSize
		add     r12,r12,r3
		lwz     r12,0(r12)
		
		// load system call address for local jump
		lwz     r0,0(r12)
		
		// r3-r8 are up to 6 arguments
		lwz     r5,32(SP)
		lwz     r6,36(SP)
		lwz     r7,40(SP)
		lwz     r8,44(SP)
		lwz     r3,48(SP)
		lwz     r4,52(SP)
		
		// jump to system call
		mtctr   r0
		bctrl
		
		// save result
		stw     r3,24(SP)
		
		bl      leave_system_call
		
		cmpi    cr0,r3,0
		
		bne-    cr0,restart
		
		// restore result
		lwz     r3,24(SP)
		
	#if CONFIG_SYSCALL_STACKS
		
		lmw     r13,64(SP)
		
	#endif
		
		// deallocate our stack frame
		lwz     SP,0(SP)
		
		// restore RTOC
		lwz     RTOC,20(SP)
		
		// standard epilog
		lwz     r0,8(SP)
		mtlr    r0
		blr
	}
	
#endif
#elif defined( __APPLE__ )
	
	void dummy()
	{
	}
	
#endif
	
}
