/*
	system_call.ppc.cc
	------------------
*/

#include "Genie/Dispatch/system_call.ppc.hh"

// Genie
#include "Genie/SystemCallRegistry.hh"


namespace Genie
{
	
#ifdef __POWERPC__
	
	enum { kSystemCallSize = sizeof (SystemCall) };
	
	asm void dispatch_ppc_system_call( ... )
	{
		// allocate a stack frame
		stwu	SP,-64(SP)
		
		// r11 contains the requested system call number
		// r3-r10 are up to 8 arguments
		
		// r12.last = gLastSystemCall;
		lwz		r12,gLastSystemCall
		lwz		r12,0(r12)
		
		// if ( r11.index > r12.last )
		cmpl	cr0,r11,r12
		blt+	cr0,in_range
		// r11.index = r12.last;
		mr		r11,r12
		
	in_range:
		// r12.array = gSystemCallArray;
		lwz		r12,gSystemCallArray
		lwz		r12,0(r12)
		
		// r12.f = r12.array[ r11.index ].function
		mulli	r11,r11,kSystemCallSize
		add		r12,r12,r11
		lwz		r12,0(r12)
		
		// load system call address for local jump
		lwz		r0,0(r12)
		
		// jump to system call
		mtctr	r0
		bctrl
		
		// deallocate our stack frame
		addi	SP,SP,64
		
		// restore RTOC
		lwz		RTOC,20(SP)
	}
	
#endif
	
}

