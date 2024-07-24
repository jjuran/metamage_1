/*	===========
 *	Platform.cc
 *	===========
 */

#include "A-line/Platform.hh"

// Mac OS
#if defined(__MACOS__) || defined(__APPLE__)
#include <TargetConditionals.h>
#endif


static
void ApplyPlatformImplications( Platform& platform )
{
	
	if ( platform & (runtimeA4CodeResource | runtimeA5CodeSegments) )
	{
		platform |= arch68K;
	}
	
	if ( platform & arch68K )
	{
		platform |= apiMacBlue;
	}
	
	if ( platform & runtimeMachO )
	{
		platform |= apiMacCarbon;
		platform |= platformUnix;
	}
}

void ApplyPlatformDefaults( Platform& platform )
{
#if ALINE_CROSS_DEVELOPMENT
	
	// FIXME for 64-bit
	if ( (platform & archMask) == 0 )
	{
		if ( TARGET_CPU_68K )
		{
			platform |= arch68K;
			platform |= apiMacBlue;
			
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
		platform |= TARGET_API_MAC_CARBON ? apiMacCarbon : apiMacBlue;
	}
	
	platform |= platformMac;
	
	ApplyPlatformImplications( platform );
	
#else
	
	if ( (platform & archMask) == 0 )
	{
		platform |= archX86;  // assumed for Linux at the moment
	}
	
	platform |= platformUnix;
	
#endif
}
