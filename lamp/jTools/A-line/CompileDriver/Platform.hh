/*	===========
 *	Platform.hh
 *	===========
 */

#ifndef COMPILEDRIVER_PLATFORM_HH
#define COMPILEDRIVER_PLATFORM_HH

#if defined(__APPLE_CC__) || defined(__MWERKS__)
#include <TargetConditionals.h>
#endif

/*

arch-runtime-api

// Zero

*-*-*         -- Set based on TARGET_CPU_* to 68k-a5-classic or ppc-*-* (see below).

// One

*-*-classic   -- Set based on TARGET_CPU_*.  If 68K, see below.

*-cfm-*       -- Set based on TARGET_CPU_*.  If PPC, see below.

ppc-*-*       -- Set based on TARGET_RT_MAC_MACHO and TARGET_API_MAC_CARBON

// Two

68k-*-classic -- Assume that a4 and cfm would have been specified; set to a5

*-cfm-classic -- Set current CPU architecture (not likely to be 68K, but theoretically possible)

ppc-cfm-*     -- Set based on TARGET_API_MAC_CARBON

ppc-*-carbon  -- Set based on TARGET_RT_MAC_MACHO

// Three

68k-a4-classic
68k-a5-classic
68k-cfm-classic
ppc-cfm-classic
ppc-cfm-carbon
ppc-macho-carbon


*/

namespace CompileDriver
{
	
	enum Architecture
	{
		archUnspecified,
		arch68K,
		archPPC,
		archX86
	};
	
	enum Runtime
	{
		runtimeUnspecified,
		runtimeA4CodeResource,
		runtimeA5CodeSegments,
		runtimeCodeFragments,
		runtimeMachO,
		runtimeELF
	};
	
	enum MacAPI
	{
		apiUnspecified,
		apiMacToolbox,
		apiMacCarbon,
		apiNotApplicable
	};
	
	class IncompatiblePlatformAttributes {};
	
	template < class Attribute >
	Attribute MergeAttributes( Attribute a, Attribute b )
	{
		if ( int( a ) == 0 )  return b;
		if ( int( b ) == 0 )  return a;
		
		if ( a == b )  return a;
		
		throw IncompatiblePlatformAttributes();
	}
	
	struct Platform
	{
		Architecture arch;
		Runtime runtime;
		MacAPI api;
		
		Platform() 
		:
			arch   ( archUnspecified    ),
			runtime( runtimeUnspecified ),
			api    ( apiUnspecified     )
		{}
		
		Platform( Architecture arch, Runtime runtime, MacAPI api )
		:
			arch   ( arch    ),
			runtime( runtime ),
			api    ( api     )
		{}
		
		friend Platform operator&( const Platform& a, const Platform& b )
		{
			return Platform( MergeAttributes( a.arch,    b.arch    ),
			                 MergeAttributes( a.runtime, b.runtime ),
			                 MergeAttributes( a.api,     b.api     ) );
		}
		
		Platform& operator&=( const Platform& other )
		{
			Platform result = *this & other;
			*this = result;
			return *this;
		}
	};
	
	inline bool operator==( const Platform& a, const Platform& b )
	{
		return a.arch    == b.arch
		    && a.runtime == b.runtime
		    && a.api     == b.api;
	}
	
	inline bool operator!=( const Platform& a, const Platform& b )
	{
		return !( a == b );
	}
	
	
	static void ApplyPlatformDefaults( Platform& platform )
	{
	#if defined(__APPLE_CC__) || defined(__MWERKS__)
		
		// FIXME for 64-bit
		if ( platform.arch == archUnspecified )
		{
			if ( TARGET_CPU_68K )
			{
				platform.arch = arch68K;
				platform.api  = apiMacToolbox;
				
				if ( platform.runtime == runtimeUnspecified )
				{
					platform.runtime = runtimeA5CodeSegments;
				}
			}
			else if ( TARGET_CPU_PPC )
			{
				platform.arch = archPPC;
			}
			else
			{
				platform.arch = archX86;
			}
		}
		
		if ( platform.runtime == runtimeUnspecified )
		{
			platform.runtime = TARGET_RT_MAC_MACHO ? runtimeMachO : runtimeCodeFragments;
		}
		
		if ( platform.api == apiUnspecified )
		{
			platform.api = TARGET_API_MAC_CARBON ? apiMacCarbon : apiMacToolbox;
		}
		
	#else
		
		platform.arch    = archX86;  // assumed for Linux at the moment
		platform.runtime = runtimeELF;
		platform.api     = apiNotApplicable;
		
	#endif
	}
	
}

#endif

