/*	==============
 *	TargetNames.cc
 *	==============
 */

#include <cstdio>

// plus
#include "plus/var_string.hh"

#include "A-line/TargetNames.hh"


namespace tool
{
	
	class InvalidPlatform {};
	
	static const char* ArchName( Platform arch )
	{
		switch ( arch )
		{
			case arch68K:  return "68K";
			case archPPC:  return "PPC";
			case archX86:  return "X86";
		};
		
		std::fprintf( stderr, "Invalid arch: %x\n", arch );
		
		throw InvalidPlatform();
	}
	
	static const char* RuntimeName( Platform rt )
	{
		switch ( rt )
		{
			case runtimeA4CodeResource:  return "Res";
			case runtimeA5CodeSegments:  return "Code";
			case runtimeCodeFragments:   return "CFM";
			case runtimeMachO:           return "MachO";
		};
		
		return "";
	}
	
	static const char* APIName( Platform api )
	{
		switch ( api )
		{
			case apiMacBlue:    return "Blue";
			case apiMacCarbon:  return "Carbon";
			default:            return "NA";
		};
		
		return "";
	}
	
	static const char* BuildName( BuildVariety build )
	{
		switch ( build )
		{
			case buildDebug:    return "Debug";
			case buildRelease:  return "Release";
			case buildDemo:     return "Demo";
		};
		
		return "";
	}
	
	plus::string MakeTargetName( Platform      arch,
	                             Platform      runtime,
	                             Platform      api,
	                             BuildVariety  build )
	{
		plus::var_string name;
		
	#if ALINE_CROSS_DEVELOPMENT
		
		name += ArchName   ( arch    );
		name += "-";
		name += RuntimeName( runtime );
		name += "-";
		name += APIName    ( api     );
		name += "-";
		
	#endif
		
		name += BuildName( build   );
		
		return name;
	}
	
}

