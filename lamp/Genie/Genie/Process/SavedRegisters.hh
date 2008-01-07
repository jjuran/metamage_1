/*	=================
 *	SavedRegisters.hh
 *	=================
 */

#ifndef GENIE_PROCESS_SAVEDREGISTERS_HH
#define GENIE_PROCESS_SAVEDREGISTERS_HH

#ifndef __TARGETCONDITIONALS__
#include <TargetConditionals.h>
#endif


namespace Genie
{
	
	struct SavedRegisters68K
	{
		long d0, d1, d2, d3, d4, d5, d6, d7;
		long a0, a1, a2, a3, a4, a5, a6, a7;
		
		long  pc;
		short sr;
	};
	
	struct SavedRegistersPPC
	{
		long gpr[ 32 ];
		
		long lr;
		long ctr;
	};
	
#if TARGET_CPU_68K
	
	typedef SavedRegisters68K SavedRegisters;
	
#else
	
	typedef SavedRegistersPPC SavedRegisters;
	
#endif
	
}

#endif

