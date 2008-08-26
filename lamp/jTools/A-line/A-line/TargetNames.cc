/*	==============
 *	TargetNames.cc
 *	==============
 */

#include <cstdio>

#include "A-line/TargetNames.hh"


namespace ALine
{
	
	class InvalidPlatform {};
	
	std::string ArchName( CD::Platform arch )
	{
		switch ( arch )
		{
			case CD::arch68K:  return "68K";
			case CD::archPPC:  return "PPC";
			case CD::archX86:  return "X86";
		};
		
		std::fprintf( stderr, "Invalid arch: %x\n", arch );
		
		throw InvalidPlatform();
	}
	
	static std::string RuntimeName( CD::Platform rt )
	{
		switch ( rt )
		{
			case CD::runtimeA4CodeResource:  return "Res";
			case CD::runtimeA5CodeSegments:  return "Code";
			case CD::runtimeCodeFragments:   return "CFM";
			case CD::runtimeMachO:           return "MachO";
			case CD::runtimeELF:             return "ELF";
		};
		
		return "";
	}
	
	static std::string APIName( CD::Platform api )
	{
		switch ( api )
		{
			case CD::apiMacBlue:    return "Blue";
			case CD::apiMacCarbon:  return "Carbon";
			default:                return "NA";
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
	
	TargetName MakeTargetName( CD::Platform  arch,
	                           CD::Platform  runtime,
	                           CD::Platform  api,
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

