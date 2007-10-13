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
	
	typedef const struct OpaqueCodeMachO* ReturnAddrMachO;
	
#ifdef __MACH__
	
	typedef ReturnAddrMachO ReturnAddrPPC;
	
#else
	
	typedef ReturnAddrPPCFrag ReturnAddrPPC;
	
#endif
	
	typedef const struct OpaqueStackFrame* StackFramePtr;
	
	enum Architecture
	{
		kArchClassic68K   = 0,
		kArchPowerPCFrag  = 1,
		kArchCFM68K       = 0 | (1 << 4),  // not supported
		
	#ifdef __POWERPC__
		
		kArchMachO        = 1 | (1 << 4)
		
	#else
		
		kArchMachO        = 2
		
	#endif
	};
	
	struct CallRecord
	{
		union
		{
			ReturnAddr68K      addr68K;
			ReturnAddrPPCFrag  addrPPCFrag;
			ReturnAddrMachO    addrMachO;
		};
		
		Architecture arch;
		
		CallRecord()  {}
		
		CallRecord( ReturnAddr68K      addr ) : addr68K    ( addr ), arch( kArchClassic68K  )  {}
		CallRecord( ReturnAddrPPCFrag  addr ) : addrPPCFrag( addr ), arch( kArchPowerPCFrag )  {}
		CallRecord( ReturnAddrMachO    addr ) : addrMachO  ( addr ), arch( kArchMachO       )  {}
	};
	
	
	StackFramePtr GetStackFramePointer();
	
	std::vector< CallRecord > GetStackCrawl( StackFramePtr frame );
	
	std::vector< CallRecord > GetStackCrawl();
	
}

#endif

