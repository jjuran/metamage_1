/*
	system_call.68k.cc
	------------------
*/

#include "Genie/Dispatch/system_call.68k.hh"

// Genie
#include "Genie/SystemCallRegistry.hh"
#include "Genie/Dispatch/kernel_boundary.hh"


namespace Genie
{
	
	extern "C" void* global_parameter_block;
	
#ifdef __MC68K__
	
	asm void dispatch_68k_system_call( ... )
	{
		// D0 contains the system call number
		
		MOVEA.L	SP,A1
		
		LINK	A6,#0
		
		MOVE.L	A1,-(SP)  ; // push the address of the first arg
		MOVE.L	D0,-(SP)  ; // push the system call number
		
		JSR		enter_system_call
		
		MOVE.L	(SP),D0  ; // restore D0
		
		UNLK	A6
		
		CMP.W	gLastSystemCall,D0
		BLT		in_range
		
		MOVE.W	gLastSystemCall,D0
		
	in_range:
		MOVEA.L	gSystemCallArray,A0
		
	#if __MC68020__
		MOVEA.L	(A0,D0.W*8),A0
	#else
		LSL.W	#3,D0
		MOVEA.L	(A0,D0.W),A0
	#endif
		
		MOVEA.L	(SP),A4  // save the caller's return address to A4
		
		ADDQ	#4,SP
		PEA		cleanup  // install our own return address
		
		JMP		(A0)
		
	cleanup:
		MOVE.L	A4,-(SP)  // restore caller's return address
		
		MOVEA.L	global_parameter_block,A1  // user pb
		
		MOVEA.L	16(A1),A4  // restore A4 from user_pb.globals
		
		LINK	A6,#0
		
		MOVE.L	D0,-(SP)  // save D0
		
		JSR		leave_system_call
		
		MOVE.L	(SP)+,D0  // restore D0
		
		UNLK	A6
		
		RTS
	}
	
#endif
	
}

