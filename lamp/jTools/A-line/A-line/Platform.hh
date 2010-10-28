/*	===========
 *	Platform.hh
 *	===========
 */

#ifndef ALINE_PLATFORM_HH
#define ALINE_PLATFORM_HH

// ALINE_MAC_DEVELOPMENT:   true if A-line can target Mac OS or Mac OS X.
// ALINE_CROSS_DEVELOPMENT: true if A-line can cross-compile.
// ALINE_UNIX_DEVELOPMENT:  true if A-line can target native Unix.
// ALINE_RELIX_DEVELOPMENT: true if A-line can target MacRelix (maybe via Classic).

#if defined(__MACOS__) || defined(__APPLE__)
#define ALINE_MAC_DEVELOPMENT 1
#else
#define ALINE_MAC_DEVELOPMENT 0
#endif

#define ALINE_CROSS_DEVELOPMENT ALINE_MAC_DEVELOPMENT

#ifdef __GNUC__
#define ALINE_UNIX_DEVELOPMENT 1
#else
#define ALINE_UNIX_DEVELOPMENT 0
#endif

/*
	With RPC builds, any platform can host a Relix build, not just MacRelix
	and OS X / Classic.
*/
#define ALINE_RELIX_DEVELOPMENT 1

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
	
	apiMacBlue    = 1 <<  8,
	apiMacCarbon  = 1 <<  9,
	
	platformMac  = 1 << 10,
	platformUnix = 1 << 11,
	
	model_near = 1 << 12,  // affects addressing modes, not data sizes
	
	runtime_A4_near = runtimeA4CodeResource | model_near,
	runtime_A5_near = runtimeA5CodeSegments | model_near,
	
	archMask     = 0x0007,
	runtimeMask  = 0x00F8,
	apiMask      = 0x0300,
	platformMask = 0x0C00,
	model_mask   = 0x1000,
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
		Platform its_requirements;
		Platform its_prohibitions;
	
	public:
		PlatformDemands() : its_requirements(),
		                    its_prohibitions()
		{
		}
		
		PlatformDemands( Platform requirements,
		                 Platform prohibitions ) : its_requirements( requirements ),
		                                           its_prohibitions( prohibitions )
		{
		}
		
		Platform Required  () const  { return its_requirements; }
		Platform Prohibited() const  { return its_prohibitions; }
		
		bool Test( Platform platform ) const
		{
			return     (platform & its_requirements) == its_requirements
			       &&  (platform & its_prohibitions) == 0;
		}
		
		bool Contradicted() const
		{
			return (its_requirements & its_prohibitions) != 0;
		}
		
		void Verify() const
		{
			if ( Contradicted() )
			{
				throw IncompatiblePlatformAttributes();
			}
		}
		
		friend bool operator<( const PlatformDemands& a, const PlatformDemands& b )
		{
			return                                                 a.its_requirements < b.its_requirements
			       || (a.its_requirements == b.its_requirements && a.its_prohibitions < b.its_prohibitions);
		}
		
		friend bool operator==( const PlatformDemands& a, const PlatformDemands& b )
		{
			return    a.its_requirements == b.its_requirements
			       && a.its_prohibitions == b.its_prohibitions;
		}
		
		friend bool operator!=( const PlatformDemands& a, const PlatformDemands& b )
		{
			return !( a == b );
		}
		
		friend PlatformDemands operator-( const PlatformDemands& it )
		{
			return PlatformDemands( it.its_prohibitions, it.its_requirements );
		}
		
		friend PlatformDemands operator|( const PlatformDemands& a, const PlatformDemands& b )
		{
			return PlatformDemands( a.its_requirements | b.its_requirements,
			                        a.its_prohibitions | b.its_prohibitions );
		}
		
		PlatformDemands& operator|=( const PlatformDemands& it )
		{
			its_requirements |= it.its_requirements;
			its_prohibitions |= it.its_prohibitions;
			
			return *this;
		}
};

void ApplyPlatformDefaults( Platform& platform );

#endif
