/*	==============
 *	TargetNames.cc
 *	==============
 */

#include <cstdio>

#include "A-line/TargetNames.hh"


namespace tool
{
	
	class InvalidPlatform {};
	
	std::string ArchName( Platform arch )
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
	
	static std::string RuntimeName( Platform rt )
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
	
	static std::string APIName( Platform api )
	{
		switch ( api )
		{
			case apiMacBlue:    return "Blue";
			case apiMacCarbon:  return "Carbon";
			default:            return "NA";
		};
		
		return "";
	}
	
	static std::string BuildName( BuildVariety build )
	{
		switch ( build )
		{
			case buildDebug:    return "Debug";
			case buildRelease:  return "Release";
			case buildDemo:     return "Demo";
		};
		
		return "";
	}
	
	TargetName MakeTargetName( Platform      arch,
	                           Platform      runtime,
	                           Platform      api,
	                           BuildVariety  build )
	{
		TargetName name;
		
	#if ALINE_CROSS_DEVELOPMENT
		
		name += ArchName   ( arch    ) + "-";
		name += RuntimeName( runtime ) + "-";
		name += APIName    ( api     ) + "-";
		
	#endif
		
		name += BuildName( build   );
		
		return name;
	}
	
}

