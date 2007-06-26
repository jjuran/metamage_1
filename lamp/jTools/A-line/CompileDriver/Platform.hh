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
	
	/*
	enum Architecture
	{
		archUnspecified = 0,
		
		arch68K = 1 << 0,
		archPPC = 1 << 1,
		archX86 = 1 << 2
	};
	
	enum Runtime
	{
		runtimeUnspecified = 0,
		
		runtimeA4CodeResource = 1 << 3,
		runtimeA5CodeSegments = 1 << 4,
		runtimeCodeFragments  = 1 << 5,
		runtimeMachO          = 1 << 6,
		runtimeELF            = 1 << 7
	};
	
	enum MacAPI
	{
		apiUnspecified = 0,
		apiMacToolbox  = 1 <<  8,
		apiMacCarbon   = 1 <<  9,
		apiNotAMac     = 1 << 10
	};
	*/
	
	enum Platform
	{
		platformUnspecified = 0,
		
		arch68K = 1 << 0,
		archPPC = 1 << 1,
		archX86 = 1 << 2,
		
		runtimeA4CodeResource = 1 << 3,
		runtimeA5CodeSegments = 1 << 4,
		runtimeCodeFragments  = 1 << 5,
		runtimeMachO          = 1 << 6,
		runtimeELF            = 1 << 7,
		
		apiMacToolbox  = 1 <<  8,
		apiMacCarbon   = 1 <<  9,
		//apiNotAMac     = 1 << 10,
		
		archMask    = 0x0007,
		runtimeMask = 0x00F8,
		apiMask     = 0x0300
		
		
	};
	
	inline Platform operator&( Platform a, Platform b )
	{
		return Platform( int( a ) & int( b ) );
	}
	
	inline Platform operator|( Platform a, Platform b )
	{
		return Platform( int( a ) | int( b ) );
	}
	
	inline Platform& operator&=( Platform& a, Platform b )
	{
		return a = a & b;
	}
	
	inline Platform& operator|=( Platform& a, Platform b )
	{
		return a = a | b;
	}
	
	
	class IncompatiblePlatformAttributes {};
	
	
	class PlatformDemands
	{
		private:
			Platform itsRequired;
			Platform itsProhibited;
		
		public:
			PlatformDemands() : itsRequired(), itsProhibited()  {}
			
			PlatformDemands( Platform required, Platform prohibited ) : itsRequired( required ), itsProhibited( prohibited )  {}
			
			Platform Required  () const  { return itsRequired;   }
			Platform Prohibited() const  { return itsProhibited; }
			
			bool Test( Platform platform ) const
			{
				return     (platform & itsRequired  ) == itsRequired
				       &&  (platform & itsProhibited) == 0;
			}
			
			bool Contradicted() const
			{
				return itsRequired & itsProhibited;
			}
			
			void Verify() const
			{
				if ( Contradicted() )
				{
					throw IncompatiblePlatformAttributes();
				}
			}
			
			friend bool operator==( const PlatformDemands& a, const PlatformDemands& b )
			{
				return    a.itsRequired   == b.itsRequired
				       && a.itsProhibited == b.itsProhibited;
			}
			
			friend bool operator!=( const PlatformDemands& a, const PlatformDemands& b )
			{
				return !( a == b );
			}
			
			friend PlatformDemands operator-( const PlatformDemands& it )
			{
				return PlatformDemands( it.itsProhibited, it.itsRequired );
			}
			
			friend PlatformDemands operator|( const PlatformDemands& a, const PlatformDemands& b )
			{
				return PlatformDemands( a.itsRequired   | b.itsRequired,
				                        a.itsProhibited | b.itsProhibited );
			}
			
			PlatformDemands& operator|=( const PlatformDemands& it )
			{
				itsRequired   |= it.itsRequired;
				itsProhibited |= it.itsProhibited;
			}
	};
	
	
	static void ApplyPlatformDefaults( Platform& platform )
	{
	#if defined(__APPLE_CC__) || defined(__MWERKS__)
		
		// FIXME for 64-bit
		if ( (platform & archMask) == 0 )
		{
			if ( TARGET_CPU_68K )
			{
				platform |= arch68K;
				platform |= apiMacToolbox;
				
				if ( (platform & runtimeMask) == 0 )
				{
					platform |= runtimeA5CodeSegments;
				}
			}
			else if ( TARGET_CPU_PPC )
			{
				platform |= archPPC;
			}
			else
			{
				platform |= archX86;
			}
		}
		
		if ( (platform & runtimeMask) == 0 )
		{
			platform |= TARGET_RT_MAC_MACHO ? runtimeMachO : runtimeCodeFragments;
		}
		
		if ( (platform & apiMask) == 0 )
		{
			platform |= TARGET_API_MAC_CARBON ? apiMacCarbon : apiMacToolbox;
		}
		
	#else
		
		if ( (platform & archMask) == 0 )
		{
			platform |= archX86;  // assumed for Linux at the moment
		}
		
		if ( (platform & runtimeMask) == 0 )
		{
			platform |= runtimeELF;
		}
		
		if ( (platform & apiMask) == 0 )
		{
			//platform |= apiNotApplicable;
		}
		
	#endif
	}
	
}

#endif

