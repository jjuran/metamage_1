/*	=============
 *	StackCrawl.hh
 *	=============
 */

#ifndef BACKTRACE_STACKCRAWL_HH
#define BACKTRACE_STACKCRAWL_HH

// Standard C++
#include <vector>


namespace Backtrace
{
	
	typedef const struct OpaqueCode68K* ReturnAddr68K;
	
	typedef const struct OpaqueCodePPCFrag* ReturnAddrPPCFrag;
	
	typedef const struct OpaqueCodePowerPCMachO* ReturnAddrPPCMachO;
	
#ifdef __MACH__
	
	typedef ReturnAddrPPCMachO ReturnAddrPPC;
	
#else
	
	typedef ReturnAddrPPCFrag ReturnAddrPPC;
	
#endif
	
	enum Architecture
	{
		kArchClassic68K   = 0,
		kArchPowerPCFrag  = 1,
		kArchCFM68K       = 0 | (1 << 4),  // not supported
		kArchPowerPCMachO = 1 | (1 << 4),
		
	#ifdef __MACH__
		
		kArchPowerPCNative = kArchPowerPCMachO
		
	#else
		
		kArchPowerPCNative = kArchPowerPCFrag
		
	#endif
	};
	
	struct CallRecord
	{
		union
		{
			ReturnAddr68K addr68K;
			
			ReturnAddrPPCFrag addrPPCFrag;
			
			ReturnAddrPPCMachO addrPPCMachO;
		};
		
		Architecture arch;
		
		CallRecord()  {}
		
		CallRecord( ReturnAddr68K      addr ) : addr68K     ( addr ), arch( kArchClassic68K   )  {}
		CallRecord( ReturnAddrPPCFrag  addr ) : addrPPCFrag ( addr ), arch( kArchPowerPCFrag  )  {}
		CallRecord( ReturnAddrPPCMachO addr ) : addrPPCMachO( addr ), arch( kArchPowerPCMachO )  {}
	};
	
	
	std::vector< CallRecord > GetStackCrawl();
	
}

#endif

