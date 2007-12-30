/*	==============
 *	SystemCalls.cc
 *	==============
 */

// Mac OS
#include <LowMem.h>

// Lamp
#include "lamp/syscalls.h"


#pragma exceptions off


#if TARGET_CPU_68K
	
	extern "C" void InitializeProcess();
	
	enum { kDispatcherAddr = (long) LMGetToolScratch() + 4 };
	
	static asm void SystemCall()
	{
		JSR			InitializeProcess	;
		
		MOVEA.L		kDispatcherAddr,A0	;  // load the dispatcher's address
										;  // arg 1:  syscall index already on stack
		JMP			(A0)				;  // jump to dispatcher -- doesn't return
		
		// Not reached
	}
	
	#define DEFINE_STUB( name ) extern "C" void name(); asm void name() { MOVE.L #__NR_##name,-(SP) ;  JMP SystemCall }
	
#endif

#if TARGET_CPU_PPC
	
	#pragma export on
		
		void    (*gDispatcher)();
		
	#pragma export reset
	
	static asm void SystemCall()
	{
		mflr	r0				// get caller's return address
		stw		r0,8(SP)		// store return address in caller's stack frame
		
		stwu	SP,-64(SP)		// allocate our own stack frame
		
		stw		RTOC,20(SP)		// save RTOC into stack
		
		stw		r3,24(SP)		// save system call arguments into stack
		stw		r4,28(SP)
		stw		r5,32(SP)
		stw		r6,36(SP)
		stw		r7,40(SP)
		stw		r8,44(SP)
		stw		r9,48(SP)
		stw		r10,52(SP)
		
		lwz		r12,gDispatcher	// load address of export
		lwz		r12,0(r12)		// load dispatcher T-vector
		lwz		r0,0(r12)		// load function address
		lwz		RTOC,4(r12)		// load foreign RTOC
		mtctr	r0				// ready, aim...
		bctrl					// branch to dispatcher (with return link)
		lwz		RTOC,20(SP)		// restore our RTOC
		
		addi	SP,SP,64		// deallocate our stack frame
		
		lwz		r0,8(SP)		// reload caller's return address
		mtlr	r0				// load it into the link register
		blr						// return
	}
	
	#define DEFINE_STUB( name ) extern "C" void name(); asm void name() { addi r11,0,__NR_##name ;  b SystemCall }
	
#endif

DEFINE_STUB( read  )
DEFINE_STUB( write )
DEFINE_STUB( open  )
DEFINE_STUB( close )

