/*	==============
 *	SystemCalls.cc
 *	==============
 */

// Mac OS
#include <LowMem.h>


#pragma exceptions off


typedef int (*Dispatcher)( const char* );

struct ToolScratchGlobals
{
	Dispatcher       dispatcher;
	void*            reserved;
};


#if TARGET_CPU_68K
	
	extern "C" void InitializeProcess();
	
	enum { kDispatcherAddr = (long) LMGetToolScratch() };
	
	inline void PushAddress( const void* ptr )
	{
		asm
		{
			MOVEA.L		ptr,A0	;
			PEA			(A0)	;
		}
	}
	
	inline asm void SystemCall()
	{
		MOVEA.L		kDispatcherAddr,A0	;  // load the dispatcher's address
										;  // arg 1:  function name C string already on stack
		JSR			(A0)				;  // jump to dispatcher -- doesn't return
		
		// Not reached
	}
	
#endif

#if TARGET_CPU_PPC
	
	static asm void PPCSystemCall()
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
		
		lwz		r12,0x09CE		// retrieve dispatcher T-vector from ToolScratch
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
	
#endif


extern "C" void write();

static const char* name_write = "write";

#if TARGET_CPU_68K
	
	void write()
	{
		InitializeProcess();
		PushAddress( name_write );
		SystemCall();
	}
	
#endif

#if TARGET_CPU_PPC
	
	asm void write()
	{
		lwz		r11,name_write
		b		PPCSystemCall
	}
	
#endif

