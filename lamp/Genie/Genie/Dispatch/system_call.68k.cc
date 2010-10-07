/*
	system_call.68k.cc
	------------------
*/

#include "Genie/Dispatch/system_call.68k.hh"

// Genie
#include "Genie/SystemCallRegistry.hh"


namespace Genie
{
	
#ifdef __MC68K__
	
	asm void dispatch_68k_system_call( ... )
	{
		// D0 contains the system call number
		
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
		
		JMP		(A0)
	}
	
#endif
	
}

