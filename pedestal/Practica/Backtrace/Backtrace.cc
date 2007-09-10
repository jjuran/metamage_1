/*	============
 *	Backtrace.cc
 *	============
 */

#include "Backtrace/Backtrace.hh"

// Standard C++
#include <string>

// Standard C/C++
#include <cstdio>

// Backtrace
#include "Backtrace/MachO.hh"
#include "Backtrace/MacsbugSymbols.hh"
#include "Backtrace/TracebackTables.hh"
#include "Backtrace/Unmangle.hh"


#ifndef TARGET_CPU_PPC
	#ifdef __POWERPC__
		#define TARGET_CPU_PPC 1
	#else
		#define TARGET_CPU_PPC 0
	#endif
#endif


namespace Backtrace
{
	
	template < class SymbolPtr >
	inline std::string GetNameFromSymbolPtr( SymbolPtr symbol )
	{
		return symbol != NULL ? GetSymbolString( symbol ) : "???";
	}
	
	template < class ReturnAddr >
	inline std::string FindSymbolString( ReturnAddr addr )
	{
		return GetNameFromSymbolPtr( FindSymbolName( addr ) );
	}
	
	template < class ReturnAddr >
	inline std::string GetSymbolName( ReturnAddr addr )
	{
		return FindSymbolString( addr );
	}
	
	template <>
	inline std::string GetSymbolName< ReturnAddrPPCFrag >( ReturnAddrPPCFrag addr )
	{
		const ReturnAddrPPCFrag mixedModeSwitch = (ReturnAddrPPCFrag) 0xffcec400;
		
		return addr == mixedModeSwitch ? "MixedMode" : FindSymbolString( addr );
	}
	
	static void PrintTrace( unsigned offset, const void* addr, const char* arch, const char* name )
	{
		std::fprintf( stderr, "%d: 0x%.8x (%s) %s\n", offset, addr, arch, name );
	}
	
	template < class ReturnAddr >
	inline void TraceAddress( unsigned offset, ReturnAddr addr, const char* arch )
	{
		PrintTrace( offset, addr, arch, GetSymbolName( addr ).c_str() );
	}
	
	DebuggingContext::DebuggingContext() : itsStackCrawl( GetStackCrawl() )
	{
	}
	
	void DebuggingContext::Show() const
	{
		unsigned levelsToSkip = 0;
		
		typedef std::vector< CallRecord >::const_iterator Iter;
		
		const std::vector< CallRecord >& trace = itsStackCrawl;
		
		const Iter begin = trace.begin() + levelsToSkip;
		
		unsigned offset = 0;
		
		// It's important to use < instead of != if we might skip past the end
		for ( Iter it = begin;  it < trace.end();  ++it, ++offset )
		{
			const CallRecord& call = *it;
			
			switch ( call.arch )
			{
				case Backtrace::kArchClassic68K:
					Backtrace::TraceAddress( offset, call.addr68K, "68K" );
					break;
				
				case Backtrace::kArchPowerPCFrag:
					Backtrace::TraceAddress( offset, call.addrPPCFrag, "PPC" );
					break;
				
				case Backtrace::kArchMachO:
					Backtrace::TraceAddress( offset, call.addrMachO, TARGET_CPU_PPC ? "PPC" : "X86" );
					break;
				
				default:
					std::fprintf( stderr, "Trace: architecture %x for addres %.8x is unknown.\n", call.arch, call.addr68K );
					break;
			}
		}
	}
	
}

