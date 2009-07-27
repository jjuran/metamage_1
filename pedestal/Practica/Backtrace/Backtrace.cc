/*	============
 *	Backtrace.cc
 *	============
 */

#include "Backtrace/Backtrace.hh"

// Standard C++
#include <algorithm>
#include <functional>
#include <string>

// Standard C/C++
#include <cstdio>

#ifdef __GNUC__
#include <cxxabi.h>
#endif

// Backtrace
#include "Backtrace/Filter.hh"
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
	
#ifdef __MACOS__
	
	template <> struct UnmanglingForReturnAddr_Traits< ReturnAddr68K >
	{
		static std::string Unmangle( const std::string& name )  { return UnmangleMWC68K( name ); }
	};
	
	template <> struct UnmanglingForReturnAddr_Traits< ReturnAddrCFM >
	{
		static std::string Unmangle( const std::string& name )  { return UnmangleMWCPPC( name ); }
	};
	
#endif
	
#ifdef __GNUC__
	
	template <> struct UnmanglingForReturnAddr_Traits< ReturnAddrNative >
	{
		static std::string Unmangle( const std::string& name )
		{
			std::string result = name;
			
			// s/:.*//;
			result.resize( std::find( result.begin(), result.end(), ':' ) - result.begin() );
			
			const char* mangled_name = result.c_str();
			
			if ( std::equal( mangled_name, mangled_name + 3, "__Z" ) )
			{
				++mangled_name;
			}
			
			if ( char* unmangled = abi::__cxa_demangle( mangled_name, NULL, NULL, NULL ) )
			{
				result = unmangled;
				
				free( unmangled );
				
				return result;
			}
			
			return result;
		}
	};
	
#endif
	
#ifdef __ELF__
	
	static const char* FindSymbolName( ReturnAddrNative )
	{
		return NULL;
	}
	
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
	
#ifdef __MACOS__
	
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
	
	CallInfo GetCallInfoFromReturnAddress( const FrameData& call )
	{
		CallInfo result;
		
		result.itsFramePtr   = call.framePtr;
		result.itsReturnAddr = call.addrNative;
		
	#ifdef __MACOS__
		
		result.itsArch          = call.isCFM ? "ppc" : "68k";
		result.itsUnmangledName = call.isCFM ? GetUnmangledSymbolName( call.addrCFM    )
		                                     : GetUnmangledSymbolName( call.addrNative );
		
		result.itsUnmangledName = FilterSymbol( result.itsUnmangledName );
		
	#else
		
		result.itsArch          = TARGET_CPU_PPC ? "ppc" : "x86";
		result.itsUnmangledName = GetUnmangledSymbolName( call.addrNative );
		
	#endif
		
		return result;
	}
	
	static std::string MakeReportForCall( unsigned            offset,
	                                      const void*         frame,
	                                      const void*         addr,
	                                      const char*         arch,
	                                      const std::string&  name )
	{
		char buffer[ sizeof "1234567890: [<0x12345678|xyz>] \0" ];
		
		std::sprintf( buffer, "%2d: [<%#.8x|%s>] \0", offset, addr, arch );
		
		std::string result = buffer;
		
		result += name;
		result += "\n";
		
		return result;
	}
	
	std::string MakeReportFromCallChain( std::vector< CallInfo >::const_iterator  begin,
	                                     std::vector< CallInfo >::const_iterator  end )
	{
		unsigned offset = 0;
		
		std::string result;
		
		// It's important to use < instead of != if we might skip past the end
		for ( std::vector< CallInfo >::const_iterator it = begin;  it < end;  ++it, ++offset )
		{
			const CallInfo& info = *it;
			
			result += MakeReportForCall( offset, info.itsFramePtr,
			                                     info.itsReturnAddr,
			                                     info.itsArch,
			                                     info.itsUnmangledName );
		}
		
		result += "\n";
		
		return result;
	}
	
	std::string MakeReportFromStackCrawl( std::vector< FrameData >::const_iterator  begin,
	                                      std::vector< FrameData >::const_iterator  end )
	{
		std::vector< CallInfo > callChain;
		
		callChain.resize( end - begin );
		
		std::transform( begin, end, callChain.begin(), std::ptr_fun( GetCallInfoFromReturnAddress ) );
		
		return MakeReportFromCallChain( callChain.begin(), callChain.end() );
	}
	
	static const void* gStackBottomLimit = (const void*) 0xFFFFFFFF;
	
	DebuggingContext::DebuggingContext() : itsStackCrawl( MakeStackCrawlToBottom( gStackBottomLimit ) )
	{
	}
	
	void SetStackBottomLimit( const void* limit )
	{
		gStackBottomLimit = limit;
	}
	
}

