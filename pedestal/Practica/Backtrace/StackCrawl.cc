/*	=============
 *	StackCrawl.cc
 *	=============
 */

#include "Backtrace/StackCrawl.hh"


namespace Backtrace
{
	
	struct StackFrame68K
	{
		StackFrame68K*  next;
		ReturnAddr68K   returnAddr;
	};
	
	struct StackFramePPC
	{
		StackFramePPC*  next;
		const void*     savedCR;
		ReturnAddrPPC   returnAddr;
	};
	
	
#ifdef __MC68K__
	
	#pragma parameter __D0 GetA6
	
	inline char* GetA6() = { 0x200e };
	
	inline const StackFrame68K* GetTopFrame()  { return (const StackFrame68K*) GetA6(); }
	
#endif

#ifdef __POWERPC__
	
	static asm char *GetSP( void )
	{
			mr		r3,r1
		#if !defined(__MWERKS__)	
			blr
		#endif
	}
	
	inline const StackFramePPC* GetTopFrame()  { return ( (const StackFramePPC*) GetSP() )->next; }
	
#endif
	
	static const StackFramePPC* SwitchBackToPPCFrom68K( const StackFrame68K* frame )
	{
		const StackFramePPC* switchFrame = (const StackFramePPC*) frame;
		
		return switchFrame;
	}
	
	static const StackFrame68K* SwitchBackTo68KFromPPC( const StackFramePPC* frame )
	{
		const StackFrame68K* switchFrame = (const StackFrame68K*) ((long) frame - 1);
		
		return switchFrame;
	}
	
	static void CrawlStackPPC( const StackFramePPC* frame, std::vector< CallRecord >& result );
	
	static void CrawlStack68K( const StackFrame68K* frame, std::vector< CallRecord >& result )
	{
		if ( frame == NULL )
		{
			return;
		}
		
		if ( *((unsigned long*) frame - 1) == 0xffffffff )
		{
			const StackFramePPC* switchFrame = SwitchBackToPPCFrom68K( frame );
			
			CrawlStackPPC( switchFrame, result );
			
			return;
		}
		
		ReturnAddr68K addr = frame->returnAddr;
		
		result.push_back( CallRecord( addr ) );
		
		if ( frame->next < frame )
		{
			return;
		}
		
		CrawlStack68K( frame->next, result );
	}
	
	static void CrawlStackPPC( const StackFramePPC* frame, std::vector< CallRecord >& result )
	{
		if ( frame == NULL )
		{
			return;
		}
		
		if ( (long) frame & 0x00000001 )
		{
			const StackFrame68K* switchFrame = SwitchBackTo68KFromPPC( frame );
			
			CrawlStack68K( switchFrame, result );
			
			return;
		}
		
		ReturnAddrPPC addr = frame->returnAddr;
		
		result.push_back( CallRecord( addr ) );
		
		if ( frame->next < frame )
		{
			return;
		}
		
		CrawlStackPPC( frame->next, result );
	}
	
	inline void CrawlStack( const StackFrame68K* frame, std::vector< CallRecord >& result )
	{
		CrawlStack68K( frame, result );
	}
	
	inline void CrawlStack( const StackFramePPC* frame, std::vector< CallRecord >& result )
	{
		CrawlStackPPC( frame, result );
	}
	
	std::vector< CallRecord > GetStackCrawl()
	{
		std::vector< CallRecord > result;
		
		CrawlStack( GetTopFrame(), result );
		
		return result;
	}
	
}

