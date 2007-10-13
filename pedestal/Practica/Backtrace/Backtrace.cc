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
	
	template < class ReturnAddr > struct UnmanglingForReturnAddr_Traits;
	
#if defined( __MACOS__ ) && !defined( __MACH__ )
	
	template <> struct UnmanglingForReturnAddr_Traits< ReturnAddr68K >
	{
		static std::string Unmangle( const std::string& name )  { return UnmangleMWC68K( name ); }
	};
	
	template <> struct UnmanglingForReturnAddr_Traits< ReturnAddrCFM >
	{
		static std::string Unmangle( const std::string& name )  { return UnmangleMWCPPC( name ); }
	};
	
#endif
	
#ifdef __MACH__
	
	template <> struct UnmanglingForReturnAddr_Traits< ReturnAddrNative >
	{
		static std::string Unmangle( const std::string& name )  { return UnmangleGCC( name ); }
	};
	
#endif
	
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
	
#if defined( __MACOS__ ) && !defined( __MACH__ )
	
	template <>
	inline std::string GetSymbolName< ReturnAddrPPC >( ReturnAddrPPC addr )
	{
		const ReturnAddrPPC mixedModeSwitch = (ReturnAddrPPC) 0xffcec400;
		
		return addr == mixedModeSwitch ? "MixedMode" : FindSymbolString( addr );
	}
	
#endif
	
	template < class ReturnAddr >
	inline std::string GetUnmangledSymbolName( ReturnAddr addr )
	{
		std::string name = GetSymbolName( addr );
		
		try
		{
			return UnmanglingForReturnAddr_Traits< ReturnAddr >::Unmangle( name );
		}
		catch ( ... )
		{
			return name;
		}
	}
	
	TraceRecord TraceCall( const CallRecord& call )
	{
		TraceRecord result;
		
		result.itsReturnAddr = call.addrNative;
		
	#if defined( __MACOS__ ) && !defined( __MACH__ )
		
		result.itsArch          = call.isCFM ? "PPC" : "68K";
		result.itsUnmangledName = call.isCFM ? GetUnmangledSymbolName( call.addrCFM    )
		                                     : GetUnmangledSymbolName( call.addrNative );
		
	#else
		
		result.itsArch          = TARGET_CPU_PPC ? "PPC" : "X86";
		result.itsUnmangledName = GetUnmangledSymbolName( call.addrNative );
		
	#endif
		
		return result;
	}
	
	static void PrintTrace( unsigned offset, const void* addr, const char* arch, const char* name )
	{
		std::fprintf( stderr, "%d: 0x%.8x (%s) %s\n", offset, addr, arch, name );
	}
	
	DebuggingContext::DebuggingContext() : itsStackCrawl( GetStackCrawl() )
	{
	}
	
	void DebuggingContext::Show() const
	{
		unsigned levelsToSkip = 1;
		
		typedef std::vector< CallRecord >::const_iterator Iter;
		
		const std::vector< CallRecord >& trace = itsStackCrawl;
		
		const Iter begin = trace.begin() + levelsToSkip;
		
		unsigned offset = 0;
		
		// It's important to use < instead of != if we might skip past the end
		for ( Iter it = begin;  it < trace.end();  ++it, ++offset )
		{
			const CallRecord& call = *it;
			
			TraceRecord trace = TraceCall( call );
			
			PrintTrace( offset, trace.itsReturnAddr,
			                    trace.itsArch,
			                    trace.itsUnmangledName.c_str() );
		}
	}
	
}

