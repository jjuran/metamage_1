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
	typedef const struct OpaqueCodePPC* ReturnAddrPPC;
	
	union ReturnAddr
	{
		ReturnAddr68K addr68K;
		ReturnAddrPPC addrPPC;
	};
	
	enum Architecture
	{
		kArchClassic68K = 0,
		kArchPowerPCCFM = 1,
		kArchCFM68K     = 1 << 4  // not supported
	};
	
	struct CallRecord
	{
		union
		{
			ReturnAddr68K addr68K;
			ReturnAddrPPC addrPPC;
		};
		
		Architecture arch;
		
		CallRecord()  {}
		
		CallRecord( ReturnAddr68K addr ) : addr68K( addr ), arch( kArchClassic68K )  {}
		CallRecord( ReturnAddrPPC addr ) : addrPPC( addr ), arch( kArchPowerPCCFM )  {}
	};
	
	
	std::vector< CallRecord > GetStackCrawl();
	
}

#endif

